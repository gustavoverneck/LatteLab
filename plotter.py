import os
import matplotlib.pyplot as plt
import numpy as np
from multiprocessing import Pool

os.makedirs("exports", exist_ok=True)  # Creates the folder if it doesn't exist

class Data:
    def __init__(self, filename, title="", size=100):
        self.filename = filename
        self.data = self.read_data()
        self.title = title
        self.size = size
        self.cmap = "inferno"
    
    def read_data(self):
        try:
            with open(self.filename, 'r') as f:
                data = f.readlines()
            return data
        except FileNotFoundError:
            print(f"File {self.filename} not found.")
            return None
    
    def interpret_data(self):
        if self.data is None:
            return None
        x = []
        y = []
        z = []
        rho = []
        ux = []
        uy = []
        uu = []
        for line in self.data[1:]:
            line = line.split(",\t")
            x.append(float(line[0]))
            y.append(float(line[1]))
            z.append(float(line[2]))
            rho.append(float(line[3]))
            ux.append(float(line[4]))
            uy.append(float(line[5]))
            uu.append(np.sqrt(float(line[4])**2 + float(line[5])**2))
        
        self.x = np.array(x)
        self.y = np.array(y)
        self.rho = np.array(rho)
        self.ux = np.array(ux)
        self.uy = np.array(uy)
        self.uu = np.array(uu)

        pass
    
    def plot_export(self, field='rho'):
        filename = self.title
        x, y = np.meshgrid(np.linspace(min(self.x), max(self.x), self.size), np.linspace(min(self.y), max(self.y), self.size))
        k = np.reshape(self.uu, (self.size, self.size))
        u = np.reshape(self.ux, (self.size, self.size))
        v = np.reshape(self.uy, (self.size, self.size))
        if field == 'rho':
            print("Plotting density for ", filename)
            plt.clf()
            plt.scatter(self.x, self.y, c=self.rho, cmap=self.cmap)
            plt.colorbar()
            plt.title('Density')
            plt.savefig(f"exports/rho_{filename}.png", dpi=600)
        elif field == 'u':
            print("Plotting velocity for ", filename)
            plt.clf()
            plt.scatter(self.x, self.y, c=self.uu, cmap=self.cmap)
            plt.colorbar()
            plt.title('Velocity')
            plt.savefig(f"exports/u_{filename}.png", dpi=600)
        elif field == 'streamlines':
            print("Plotting streamlines for ", filename)
            plt.clf()
            plt.scatter(self.x, self.y, c=self.uu, cmap=self.cmap)
            plt.streamplot(x, y, u, v, density=3, color='white', linewidth=0.1, cmap=self.cmap)
            plt.title('Streamlines')
            plt.savefig(f"exports/streamlines_{filename}.png", dpi=600)
        elif field == 'contour':
            print("Plotting contour for ", filename)
            plt.clf()
            levels = 20
            cmap = plt.get_cmap(self.cmap, levels+1)
            plt.contourf(x, y, k, levels, cmap=self.cmap)
            plt.streamplot(x, y, u, v, density=3, color='white', linewidth=0.1, cmap=self.cmap)
            plt.title('Streamlines')
            plt.savefig(f"exports/contour_{filename}.png", dpi=600)
        else:
            print('Invalid field')
            return None
        

def process_single_file(value):
    global size
    data = Data(f'exports/data_{value}.csv', title=value, size=size)
    data.interpret_data()
    data.plot_export('rho')
    data.plot_export('u')
    data.plot_export('streamlines')
    data.plot_export('contour')

if __name__ == '__main__':
    # Parameters
    size = 128
    timesteps = 1000
    interval = 100
    t0 = 100
    
    file_list = np.arange(t0, timesteps+1, interval)
    with Pool() as pool:
        pool.map(process_single_file, file_list)