#include <GL/glut.h>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

const int HEIGHT = 1000, WIDTH = 1000, DEPTH = 1000; // dimensions
const int DIST = 200; // manhattan distance

struct Point{
    double x,y,z;
    Point() : x(0), y(0), z(0){}
    Point(double x, double y, double z) : x(x),y(y),z(z){}

    Point projection(){
        Point projected;
        projected.x = x / WIDTH;
        projected.y = y / HEIGHT;
        projected.z = z / DEPTH;
        return projected;
    }
};
struct Cube{
    struct Face{
        Point points[4]; 
        GLfloat colors[3];

        Face(){}
        Face(Point pt1, Point pt2, Point pt3, Point pt4){
            points[0] = pt1;
            points[1] = pt2;
            points[2] = pt3;
            points[3] = pt4;
            for(int j=0; j<3; j++) colors[j] = rand()/(RAND_MAX + 1.0);
        }

        Face projection(){
            Face projected;
            for(int j=0; j<4; j++) projected.points[j] = points[j].projection();
            for(int j=0; j<3; j++) projected.colors[j] = colors[j];
            return projected;
        }
    };
    /*
        front face : faces[0]
        up face    : faces[1]
        back face  : faces[2]
        down face  : faces[3]
        left face  : faces[4]
        right face : faces[5]
    */
    Face faces[6];
    Face projected_faces[6];
    double rotation[3] = {0,0,0};
    const double rotation_value = 5;

    /*
        First 4 points are the coordinates of the front face,
        Last 4 points are the coordinates of the back face.
        The points of each face are given in counterclockwise order. Starting from the uppermost and leftmost point  
     */  
    Cube(Point pt1, Point pt2, Point pt3, Point pt4,
              Point pt5, Point pt6, Point pt7, Point pt8){
        // front face
        faces[0] = Face(pt1,pt2,pt3,pt4);
        // up face
        faces[1] = Face(pt5, pt6,pt2,pt1);
        // back face
        faces[2] = Face(pt5,pt6,pt7,pt8);
        // down face
        faces[3] = Face(pt8,pt7,pt3,pt4);
        // left face
        faces[4] = Face(pt2,pt6,pt7,pt3);
        // right face
        faces[5] = Face(pt5,pt1,pt4,pt8);
        
        for(int j=0; j<6; j++) projected_faces[j] = faces[j].projection();
    }
    
    void draw(){
        glLoadIdentity();        
        glRotatef(rotation[0], 1, 0, 0);
        glRotatef(rotation[1], 0, 1, 0);
        glRotatef(rotation[2], 0, 0, 1);
        
        glBegin(GL_QUADS);
        

        // draw the cube one face at a time
        for(int i=0; i<6; i++){
            auto & face = projected_faces[i];
            glColor3fv(face.colors);
            for(int j=0; j<4; j++) glVertex3f(face.points[j].x,face.points[j].y,face.points[j].z);
        }
        glEnd();
    }

    // for each axis, the cube is rotated by rotation_value*axis_name degrees
    void rotate(double x,double y,double z){
        rotation[0] += x*rotation_value;
        rotation[1] += y*rotation_value;
        rotation[2] += z*rotation_value;
    }
};

Cube cube(Point(DIST,DIST,DIST),Point(-DIST,DIST,DIST),Point(-DIST,-DIST,DIST),Point(DIST,-DIST,DIST),
          Point(DIST,DIST,-DIST),Point(-DIST,DIST,-DIST),Point(-DIST,-DIST,-DIST),Point(DIST,-DIST,-DIST)); // cube drawn on the window

void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // resets the frame

    cube.draw(); // draw the cube

    glutSwapBuffers(); // show the cube
}

void initialize() {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Projecao Ortogonal");
    glClearColor(0.0, 0.0, 0.0, 1.0);
    // when the glOrtho is not specified, the range of each axis is [-1,1]

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}


//lida com as setinhas do teclado(para rotacionar)
void keyboard(int key, int x, int y) {
    if (key == GLUT_KEY_UP) cube.rotate(1,0,0);
    if (key == GLUT_KEY_DOWN) cube.rotate(-1,0,0);
    if (key == GLUT_KEY_LEFT) cube.rotate(0,1,0);
    if (key == GLUT_KEY_RIGHT) cube.rotate(0,-1,0);

    glutPostRedisplay();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    srand(time(0));

    initialize();

    glutDisplayFunc(display);
    glutSpecialFunc(keyboard);

    glutMainLoop();
    return 0;
}
