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
Vector3f position(0, 5, 0);
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
		if (DY<0 && distanceFromObject > 2) move(-1, 0, 0);
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
 
Model_OBJ table;
Model_OBJ ball_0;
Model_OBJ ball_1;
Model_OBJ ball_2;
Model_OBJ ball_3;
Model_OBJ chairs;
Model_OBJ room;
glutWindow win;
map<string, texture> textures;
 
 

float ImageVertices[] = {
-0.125526, 1.407483, -1.984869,
1.309474, 1.407483, -1.984869,
1.309474, 0.407483, -1.984869,
-0.125526, 0.407483, -1.984869
};

float Text_Coords[] = {
0.000000, 0.000000,
1.000000, 0.000000,
1.000000, 1.000000,
0.000000, 1.000000
};

float DoorVerticies[] = {	
-1.633465, 0.007121, -0.677675,
-1.133465, 0.007121, -0.677675,
-1.133465, 0.007121, -1.857675,
-1.633465, 0.007121, -1.857675

};

//float DoorVerticies[] = {
//-1.633465, 0.004256, -1.986908,
//-1.133465, 0.004256, -1.986908,
//-1.633465, 1.184256, -1.986908,
//-1.133465, 1.184256, -1.986908
//};
					
void loadDoorAndImage()
{
	texture tex;
	string textureName = "BacHo.jpg";
	string texturePath = "resource/" + textureName;
	tex.image = SOIL_load_image(texturePath.c_str(), &(tex.width), &(tex.height), NULL, 0);
	textures.insert(textures.find(textureName), pair<string, texture>(textureName, tex));
	
//	texture door;
//	textureName = "door.jpg";
//	texturePath = "resource/" + textureName;
//	door.image = SOIL_load_image(texturePath.c_str(), &(door.width), &(door.height), NULL, 0);
//	textures.insert(textures.find(textureName), pair<string, texture>(textureName, door));
}

void drawDoorAndImage()
{
	glEnableClientState(GL_VERTEX_ARRAY);
 	glEnableClientState (GL_TEXTURE_COORD_ARRAY);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);		
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glEnable(GL_TEXTURE_2D);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (textures.find("BacHo.jpg")->second).width, (textures.find("BacHo.jpg")->second).height,
				0, GL_RGB, GL_UNSIGNED_BYTE, (textures.find("BacHo.jpg")->second).image);
		 	
	glVertexPointer(3, GL_FLOAT, 0, ImageVertices);
	glTexCoordPointer(2, GL_FLOAT, 0, Text_Coords);
	glDrawArrays(GL_QUADS, 0, 11);
		
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (textures.find("door.jpg")->second).width, (textures.find("door.jpg")->second).height,
//				0, GL_RGB, GL_UNSIGNED_BYTE, (textures.find("door.jpg")->second).image);
//		 	
//	glVertexPointer(3, GL_FLOAT, 0, DoorVerticies);
//	glTexCoordPointer(2, GL_FLOAT, 0, Text_Coords);
//	glDrawArrays(GL_QUADS, 0, 11);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);	
}
 
void display() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
//	gluLookAt( -1.1,1.1,0, 0.3,0,0, 0,1,0);
	gluLookAt( position.x,position.y,position.z, -0.3,0.2774,0, 0,1,0);
	
	//	Draw objects
	table.draw();
	chairs.draw();
	room.draw();
	
	drawDoorAndImage();
	
	glTranslated(-0.3, 0.2774, 0);
	ball_0.draw();
	glTranslated(0.5, 0, 0);
	ball_1.draw();
	glTranslated(0.1, 0, 0.1);
	ball_2.draw();
	glTranslated(0, 0, -0.2);
	ball_3.draw();

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
    GLfloat light0_pos[] = {0, 2, 0, 1};
    GLfloat spot_direction[] = { -1.0, 2.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos) ;
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.5);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.5);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.2);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0);
    glEnable( GL_LIGHT0 );
    
    glLightfv( GL_LIGHT1, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT1, GL_SPECULAR, specular );
    GLfloat light1_pos[] = {0, 2, 0, 1};
	glLightfv(GL_LIGHT1, GL_POSITION, light1_pos) ;
    glEnable( GL_LIGHT1 );
    
    glLightfv( GL_LIGHT2, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT2, GL_SPECULAR, specular );
    GLfloat light2_pos[] = {0.5, 0.5, 0.5, 1};
	glLightfv(GL_LIGHT2, GL_POSITION, light2_pos) ;
    glEnable( GL_LIGHT2 );
    
    glLightfv( GL_LIGHT3, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT3, GL_SPECULAR, specular );
    GLfloat light3_pos[] = {-1.5, 1, 1, 1};
	glLightfv(GL_LIGHT3, GL_POSITION, light3_pos) ;
    glEnable( GL_LIGHT3 );
    
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

int main(int argc, char **argv) 
{
	// set window values
	win.width = 1200;
	win.height = 700;
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

    
//    createMenu();

	table = Model_OBJ("resource/pooltable.obj", 1, &textures);
	chairs = Model_OBJ("resource/chairs.obj", 0, &textures);
	room = Model_OBJ("resource/Room.obj", 1, &textures);
	
	loadDoorAndImage();
	
	ball_0 = Model_OBJ("resource/Ball0.obj", 0, &textures);
	ball_1 = Model_OBJ("resource/ball3.obj", 0, &textures);
	ball_2 = Model_OBJ("resource/ball10.obj", 0, &textures);
	ball_3 = Model_OBJ("resource/ball13.obj", 0, &textures);


	/////////////////FOR CAMERA/////////////////////////////////////////////////////////////
  	glutMouseFunc(MouseEvent);
  	glutMotionFunc(mouseMotion);
//  	glutKeyboardFunc( keyboardCam );		
  	/////////////////FOR CAMERA/////////////////////////////////////////////////////////////
	
	glutMainLoop();												// run GLUT mainloop
	return 0;
}
