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


////////////////////FOR CAMERA////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

class Vector3f {
	public:
		float x, y, z;
		Vector3f(float x, float y, float z){
			this->x = x;
			this->y = y;
			this->z = z;
		}
		void Set(float x, float y, float z){
			this->x = x;
			this->y = y;
			this->z = z;
		}
};

class ball {
	public:
		float x,y,z;
		ball(float x, float y, float z){
			this->x = x;
			this->y = y;
			this->z = z;
		}
};

ball testball(0.000,0.000, 0.000);

float PI = 3.14;
Vector3f position(0, 0, 2);
float distanceFromObject = 5;
float angleAroundObject = 0;
float pitch = 20;
float yaw = 0;
float roll;
int oldMouseX, oldMouseY;
bool mouseDown, isLeftClick, isRightClick;

float calculateHorizontalDistance(){
	return distanceFromObject*cos(pitch*PI/180);
}
float calculateVerticalDistance(){
	return distanceFromObject*sin(pitch*PI/180);
}

void calculateCameraPosition(ball Ball, float horizDistance, float verticDistance){
	float theta = 0 + angleAroundObject;
	float offsetX = horizDistance * sin(theta*PI/180);
	float offsetZ = horizDistance * cos(theta*PI/180);
	position.x = Ball.x - offsetX;
	position.z = Ball.y - offsetZ;
	position.y = Ball.y - verticDistance;
}

void calculateZoom(float command){
	//Command is 1 or -1 which equivalent to zoom in and zoom out
	distanceFromObject += command * 0.1f;
}

void calculatePitch(float MouseDY){
	//ensure that right button is down
	float pitchChange = MouseDY * 0.1f;
	pitch -= pitchChange;	
}

void calculateAngleAroundObject(float MouseDX){
	//ensure that left button is down
//	cout << MouseDX<<endl;
	float angleChange = MouseDX * 0.1f;
	angleAroundObject -= angleChange;
}

void move(float command, float MouseDY, float MouseDX){
	calculateZoom(command);
	calculatePitch(MouseDY);
	calculateAngleAroundObject(MouseDX);
	float horizontalDistance = calculateHorizontalDistance();
	float verticalDistance = calculateVerticalDistance();
	calculateCameraPosition(testball, horizontalDistance, verticalDistance);
	yaw = 180 - angleAroundObject;
}


/////////////////FOR CAMERA/////////////////////////////////////////////////////////////
void MouseEvent(int button, int state, int x, int y){
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		isLeftClick = true;
	}else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		isRightClick = true;
	}else{
		isLeftClick = false;
		isRightClick = false;
	}
	oldMouseX = x;
	oldMouseY = y;
}
 
void mouseMotion(int x, int y){
	if (isLeftClick){
		int DY = y - oldMouseY;
		int DX = x - oldMouseX;
		move(0, (float)DY, (float)DX);
		oldMouseY = y;
		oldMouseX = x;
	}
	if (isRightClick){
		int DY = y - oldMouseY;
		if (DY>0) move(1, 0, 0);
		if (DY<0) move(-1, 0, 0);
		oldMouseY = y;
	}	
}


/////////////////FOR CAMERA/////////////////////////////////////////////////////////////

////////////////////FOR CAMERA////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


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
const int numOfBall = 3;
Table table;
Ball* balls[numOfBall];
Model_OBJ chairs;
Model_OBJ room;
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
	gluLookAt( position.x,position.y,position.z, 0.3,0,0, 0,1,0);

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
	chairs.draw();
	room.draw();	
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
    GLfloat light1_pos[] = {0, 2, 0, 1};
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

void reshape (int width, int height) {  
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	GLfloat aspect = (GLfloat) win.width / win.height;
	gluPerspective(win.field_of_view_angle, aspect, win.z_near, win.z_far);
	glMatrixMode(GL_MODELVIEW);
}  

/////////// GUI /////////////////

static int window;
static int menu_id;
static int submenu_id;
static int value = 0; 
void menu(int num){
  if(num == 0){
    glutDestroyWindow(window);
    exit(0);
  }else{
    value = num;
  }
  glutPostRedisplay();
} 

void createMenu(void){     
	submenu_id = glutCreateMenu(menu);
    glutAddMenuEntry("Sphere", 2);
    glutAddMenuEntry("Cone", 3);
    glutAddMenuEntry("Torus", 4);
    glutAddMenuEntry("Teapot", 5);     
	menu_id = glutCreateMenu(menu);
    glutAddMenuEntry("Clear", 1);
    glutAddSubMenu("Draw", submenu_id);
    glutAddMenuEntry("Quit", 0);     
	glutAttachMenu(GLUT_RIGHT_BUTTON);
} 

/////////// GUI /////////////////

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
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize(win.width,win.height);
	glutCreateWindow(win.title);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape); 
	glutIdleFunc( display );
    glutKeyboardFunc( keyboard );
	initialize();


	table = Table("resource/pooltable.obj", 1, &textures);	
	chairs = Model_OBJ("resource/chairs.obj", 0, &textures);
	room = Model_OBJ("resource/Room.obj", 1, &textures);
	
	balls[0] = new Ball("resource/Ball0.obj", 0, &textures);	
	balls[0]->pos = glm::vec3(-0.1, 0.2774, 0.1);
	balls[0]->vel = glm::vec3(-0.05, 0, -0.05);
	balls[0]->acc = glm::vec3(-0.01, 0, -0.01);	
	balls[1] = new Ball("resource/Ball3.obj", 0, &textures);		
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
    

		


	/////////////////FOR CAMERA/////////////////////////////////////////////////////////////
  	glutMouseFunc(MouseEvent);
  	glutMotionFunc(mouseMotion);  	  
	glutMainLoop();												// run GLUT mainloop
	return 0;
}
