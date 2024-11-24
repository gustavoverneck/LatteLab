import os
import matplotlib.pyplot as plt
import numpy as np

os.makedirs("exports", exist_ok=True)  # Creates the folder if it doesn't exist

class Data:
    def __init__(self, filename, title=""):
        self.filename = filename
        self.data = self.read_data()
        self.title = title
    
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
            uy.append(float(line[5].split("\n")[0]))
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
        if field == 'rho':
            plt.clf()
            plt.scatter(self.x, self.y, c=self.rho, cmap='viridis')
            plt.colorbar()
            plt.title('Density')
            plt.savefig(f"exports/{filename.split(".")[0]}_rho.png", dpi=600)
        elif field == 'u':
            plt.clf()
            plt.scatter(self.x, self.y, c=self.uu, cmap='viridis')
            plt.colorbar()
            plt.title('Velocity')
            plt.savefig(f"exports/{filename.split(".")[0]}_u.png", dpi=600)
        elif field == 'streamlines':
            k = np.reshape(self.uu, (100, 100))
            u = np.reshape(self.ux, (100, 100))
            v = np.reshape(self.uy, (100, 100))
            x, y = np.meshgrid(np.linspace(min(self.x), max(self.x), 100), np.linspace(min(self.y), max(self.y), 100))
            plt.streamplot(x, y, u, v, density=1.25, color='white', linewidth=0.5, cmap='viridis')
            plt.title('Streamlines')
            plt.savefig(f"exports/{filename.split(".")[0]}_streamlines.png", dpi=600)
        else:
            print('Invalid field')
            return None
        


if __name__ == '__main__':
    data = Data('exports/data_1000.csv', title="1000")
    data.interpret_data()
    data.plot_export('rho')
    data.plot_export('u')
    data.plot_export('streamlines')