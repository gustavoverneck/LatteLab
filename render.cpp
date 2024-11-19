#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <math.h>
#include <fstream>


#include <GL/glut.h>


using namespace std; 

typedef unsigned int uint;

// Parameter
// Variables declaration
uint D = 2;
uint Q = 9;
uint Nx = 100u;
uint Ny = 100u;
uint dx = 1;
uint dy = 1;
uint dt = 1u;
uint tmax = 1000u;

float ck = dx/dt;
float cs = 1.0f/sqrt(3.0f);
float csq = cs*cs;

// Fluid properties
double u = 0.1f;
double v = 0.4f;
double Tw = 1.0f;
double alpha = 1.0f;
double omega = 1.0f/(3.0f*alpha + 0.5f);
double rho0 = 0.0f;

// Vector declaration
vector<vector<vector<double>>> f(Q, vector<vector<double>>(Nx, vector<double>(Ny)));
vector<vector<double>> rho (Nx, vector<double>(Ny));
vector<double> feq (Q);
vector<double> w (Q);



void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POINTS);
    for (uint j = 0; j < Ny; j++) {
        for (uint i = 0; i < Nx; i++) {
            double intensity = rho[i][j];
            glColor3f(intensity, 0.0, 1.0 - intensity);
            glVertex2f((float)i / Nx, (float)j / Ny);
        }
    }
    glEnd();
    glFlush();
}

void initOpenGL(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Advection Diffusion");
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 1.0, 0.0, 1.0);
    glutDisplayFunc(display);
    glutMainLoop();
}



int main () {
    // Initialize OpenGL and start the display loop
    ifstream dataFile("/home/gverneck/codes/LatteLab/data.txt");
    if (dataFile.is_open()) {
        for (uint j = 0; j < Ny; j++) {
            for (uint i = 0; i < Nx; i++) {
                dataFile >> rho[i][j];
            }
        }
        dataFile.close();
    } else {
        cerr << "Unable to open file";
        return 1;
    }
    int argc = 1;
    char* argv[1] = {(char*)"Something"};
    initOpenGL(argc, argv);
    return 0;
}