#include <GL/glut.h>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <string>

const int HEIGHT = 1000, WIDTH = 1000, DEPTH = 1000; // dimensions
const int DIST = 200; // manhattan distance
const double PI = acos(-1); 

double rad(double ang){return ang*PI/180.0;}

struct Point{
    double x,y,z;
    Point() : x(0), y(0), z(0){}
    Point(double x, double y, double z) : x(x),y(y),z(z){}

    Point projection(){
        Point projected;
        double proportion = 2-z/(DEPTH+10);
        projected.x = x * proportion / WIDTH;
        projected.y = y * proportion / HEIGHT;
        projected.z = z * proportion / DEPTH;
        return projected;
    }
    void operator +=(const Point & ot){
        x += ot.x;
        y += ot.y;
        z += ot.z;
    }
    Point operator +(const Point & ot){
        return Point(x+ot.x,y+ot.y,z+ot.z);
    }
    Point operator -(const Point &ot){
        return Point(x-ot.x,y-ot.y,z-ot.z);
    }
    // escalar product
    int operator *(const Point &ot){
        return x*ot.x + y*ot.y + z*ot.z;
    }
    // vectorial product
    Point operator ^(const Point &ot){
        return Point(y*ot.z - z*ot.y, z*ot.x - x*ot.z, x*ot.y - y*ot.x);
    }

    // angles given in degrees
    Point rotate(double angx,double angy, double angz, Point center = Point(0,0,0)){
        double sinx = sin(rad(angx));
        double cosx = cos(rad(angx));

        double siny = sin(rad(angy));
        double cosy = cos(rad(angy));

        double sinz = sin(rad(angz));
        double cosz = cos(rad(angz));
        
        return rotate(sinx,cosx,siny,cosy,sinz,cosz,center);
    }
    // passes the trigonometric functions, useful when you don't want to calculate these functions every rotation
    Point rotate(double sinx, double cosx, double siny, double cosy, double sinz, double cosz, Point center = Point(0,0,0)){
        Point pt(x-center.x,y-center.y,z-center.z);
        Point aux = pt;

        // axis x
        pt.y = aux.y*cosx - aux.z*sinx;
        pt.z = aux.y*sinx + aux.z*cosx;
        aux = pt;

        // axis y
        pt.z = aux.z*cosy - aux.x*siny;
        pt.x = aux.z*siny + aux.x*cosy;
        aux = pt;

        // axis z
        pt.x = aux.x*cosz - aux.y*sinz;
        pt.y = aux.x*sinz + aux.y*cosz;

        pt += center;
        return pt;
    }

    friend std::ostream & operator<<(std::ostream &os, const Point &pt){
    os << "(" << pt.x << "," << pt.y << "," << pt.z << ")";
    return os;
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

        friend std::ostream & operator<<(std::ostream &os, const Face &face){
            for(int j=0; j<4; j++) {
                os << face.points[j];
                if(j != 3) os << " - ";
            }
            os << std::endl;
            return os;
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
    Point center;
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
        faces[2] = Face(pt5,pt8,pt7,pt6);
        // down face
        faces[3] = Face(pt8,pt4,pt3,pt7);
        // left face
        faces[4] = Face(pt2,pt6,pt7,pt3);
        // right face
        faces[5] = Face(pt5,pt1,pt4,pt8);
        
        for(int j=0; j<6; j++) projected_faces[j] = faces[j].projection();

        center = Point((pt1.x+pt2.x)/2.0,(pt1.y+pt4.y)/2.0,(pt1.z+pt5.z)/2.0);
    }
    
    void draw(){
        // points are drawn in range [-1,1]
        glLoadIdentity();        

        // viewer vector
        Point viewer = Point(0,0,-700);

        glBegin(GL_QUADS);
        // draw the cube one face at a time
        for(int i=0; i<6; i++){
            Face & face = projected_faces[i];
            
            Point vt1 =  face.points[1] - face.points[0];
            Point vt2 =  face.points[3] - face.points[0];
            Point vtn = vt1^vt2;

            Point view = face.points[0] - viewer;

            if(vtn*view < 0){
                glColor3fv(face.colors);
                for(int j=0; j<4; j++) glVertex3f(face.points[j].x,face.points[j].y,face.points[j].z);
            }
            
        }
        glEnd();
    }

    // for each axis, the cube is rotated by rotation_value*axis_name degrees
    void rotate(double x,double y,double z){
        // rotation[0] += x*rotation_value;
        // rotation[1] += y*rotation_value;
        // rotation[2] += z*rotation_value;

        double sinx = sin(rad(rotation_value*x));
        double cosx = cos(rad(rotation_value*x));

        double siny = sin(rad(rotation_value*y));
        double cosy = cos(rad(rotation_value*y));

        double sinz = sin(rad(rotation_value*z));
        double cosz = cos(rad(rotation_value*z));

        for(int i=0; i<6; i++){
            Face &face = faces[i];
            for(int j=0; j<4; j++) face.points[j] = face.points[j].rotate(sinx,cosx,siny,cosy,sinz,cosz,center);
        }
        for(int i=0; i<6; i++) projected_faces[i] = faces[i].projection();
    }
};

// parece que quando voce usa o glLoadIdentity os pontos TEM que estar entre [-1,1]

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
    glutCreateWindow("Back-Face Detection");
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glOrtho(-WIDTH, WIDTH, -HEIGHT,HEIGHT, -DEPTH, DEPTH);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

// rotate the cube
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
