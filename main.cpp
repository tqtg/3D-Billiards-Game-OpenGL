#include <windows.h>
#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstring>
#include <vector>
#include <cmath>
#include <map>
#include <list>
#include "lib/SOIL.h"
#include "Model_OBJ.h"

using namespace std;

#define KEY_ESCAPE 27
#define MAX_MTL 10
#define MAX_OBJ 50


/************************************************************************
  Texture
 ************************************************************************/
 
typedef struct {
	unsigned char* image;
	int width;
	int height;
} texture;

map<string, texture> textures;

/************************************************************************
  Window
 ************************************************************************/
 
typedef struct {
    int width;
	int height;
	char* title;
 
	float field_of_view_angle;
	float z_near;
	float z_far;
} glutWindow;



/***************************************************************************
 * Program code
 ***************************************************************************/
 
Model_OBJ table;
Model_OBJ balls;
glutWindow win;
list<Model_OBJ> models;
 
void display() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt( -1.2,1.3,0, 0.3,0,0, 0,1,0);

	table.draw();
	balls.draw();

	glutSwapBuffers();
}


void initialize () 
{
    glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, win.width, win.height);
	GLfloat aspect = (GLfloat) win.width / win.height;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(win.field_of_view_angle, aspect, win.z_near, win.z_far);
    glMatrixMode(GL_MODELVIEW);
    glClearColor( 0.0f, 0.1f, 0.0f, 0.5f );
    glClearDepth( 1.0f );
    
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
 
    GLfloat amb_light[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
    
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, amb_light );
    
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
    GLfloat light0_pos[] = {1, 2, 1, 1};
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos) ;
    glEnable( GL_LIGHT0 );
    
    glLightfv( GL_LIGHT1, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT1, GL_SPECULAR, specular );
    GLfloat light1_pos[] = {0, 2, 0, 1};
	glLightfv(GL_LIGHT1, GL_POSITION, light1_pos) ;
//    glEnable( GL_LIGHT1 );
    
    glLightfv( GL_LIGHT2, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT2, GL_SPECULAR, specular );
    GLfloat light2_pos[] = {-1, 2, -1, 1};
	glLightfv(GL_LIGHT2, GL_POSITION, light2_pos) ;
    glEnable( GL_LIGHT2 );
    
    glShadeModel( GL_SMOOTH );
    glDepthFunc( GL_LEQUAL );
    glEnable( GL_DEPTH_TEST );
    glEnable(GL_LIGHTING);
}


void keyboard ( unsigned char key, int x, int y ) 
{
  switch ( key ) {
    case KEY_ESCAPE:        
      exit ( 0 );   
      break;      
    default:      
      break;
  }
}

int main(int argc, char **argv) 
{
	// set window values
	win.width = 900;
	win.height = 600;
	win.title = "3D Billiards";
	win.field_of_view_angle = 30;
	win.z_near = 1.0f;
	win.z_far = 500.0f;
	
	// initialize and run program
	glutInit(&argc, argv);                                      // GLUT initialization
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );  // Display Mode
	glutInitWindowSize(win.width,win.height);					// set window size
	glutCreateWindow(win.title);								// create Window
	glutDisplayFunc(display);									// register Display Function
	glutIdleFunc( display );									// register Idle Function
    glutKeyboardFunc( keyboard );								// register Keyboard Handler
	initialize();

	table = Model_OBJ("resource/pooltable.obj", 1);
	balls = Model_OBJ("resource/threeBall.obj", 0);

	glutMainLoop();												// run GLUT mainloop
	return 0;
}
