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
#include "Table.h"
#include "math.h"

using namespace std;

#define KEY_ESCAPE 27
#define MAX_MTL 10
#define MAX_OBJ 50


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
 
//Model_OBJ table;
//Model_OBJ balls;

const int numOfBall = 3;
Table table;
Ball* balls[numOfBall];


glutWindow win;
map<string, texture> textures;
 
void drawFloor()
{
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	for (GLfloat i = -2.5; i <= 2.5; i += 0.25) {
	    glVertex3f(i, 0, 2.5); glVertex3f(i, 0, -2.5);
		glVertex3f(2.5, 0, i); glVertex3f(-2.5, 0, i);
	}
	glEnd();
	glBegin(GL_LINES);
		glVertex3f(table.left,table.heigh, table.top);
		glVertex3f(table.left,table.heigh, table.bottom);		
		
		glVertex3f(table.left,table.heigh, table.bottom);				
		glVertex3f(table.right,table.heigh, table.bottom);						
		
		glVertex3f(table.right,table.heigh, table.bottom);						
		glVertex3f(table.right,table.heigh, table.top);								
		
		glVertex3f(table.right,table.heigh, table.top);								
		glVertex3f(table.left,table.heigh, table.top);
	glEnd();
}
 
void display() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt( -1.1,1.1,0, 0.3,0,0, 0,1,0);

	//	Draw floor
	drawFloor();

	//	Draw objects
	for (int i=0; i< numOfBall; ++i){
		for (int j=i+1; j< numOfBall; ++j){
			if (balls[i]->isBallHit(balls[j]) ){
				balls[i]->resToBallHit(balls[j]);
			}
		}
		table.resToBallHitTable(balls[i]);
	}

	for (int i=0; i< numOfBall; ++i) balls[i]->draw();		
	table.draw();		
	float dt= 0.1;
	for (int i=0; i< numOfBall; ++i){
		balls[i]->pos = balls[i]->pos +  balls[i]->vel*dt;				
		float stepLength = glm::length(balls[i]->vel*dt);
		float rotateAngle = stepLength*180/(M_PI*balls[i]->radius);		
		balls[i]->angle += rotateAngle;
	}	
	Sleep(10);
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
    glClearColor( 0.29, 0.3, 0.313, 1.0 );
//    glClearDepth( 1.0f );
    
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
    GLfloat light1_pos[] = {0, 5, 0, 1};
	glLightfv(GL_LIGHT1, GL_POSITION, light1_pos) ;
    glEnable( GL_LIGHT1 );
    
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

	table = Table("resource/pooltable_final.obj", 1, &textures);	
	balls[0] = new Ball("resource/Ball10.obj", 0, &textures);	
	balls[0]->pos = glm::vec3(-0.1, 0.2774, 0.1);
	balls[0]->vel = glm::vec3(-0.05, 0, -0.05);
	balls[0]->acc = glm::vec3(-0.01, 0, -0.01);	
	balls[1] = new Ball("resource/Ball10.obj", 0, &textures);		
	balls[1]->pos = glm::vec3(0.22, 0.2774, 0.1);
	balls[1]->vel = glm::vec3(-0.05, 0, -0.05);
	balls[1]->acc = glm::vec3(-0.01, 0, -0.01);			
	balls[2] = new Ball("resource/Ball10.obj", 0, &textures);		
	balls[2]->pos = glm::vec3(0.4, 0.2774, -0.15);
	balls[2]->vel = glm::vec3(-0.05, 0, -0.05);
	balls[2]->acc = glm::vec3(-0.01, 0, -0.01);		
	balls[0]->col[0] = 1.0f;
	balls[1]->col[1] = 1.0f;
	balls[2]->col[2] = 1.0f;
	
	glutMainLoop();												// run GLUT mainloop
	return 0;
}
