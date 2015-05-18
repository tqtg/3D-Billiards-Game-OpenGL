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

void keyboardCam(){
	
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
float force = 0;
float fdir = -1;
bool isHUDActive = false;

float ImageVertices[] = 
{
	-0.125526, 1.407483, -1.984869,
	1.309474, 1.407483, -1.984869,
	1.309474, 0.407483, -1.984869,
	-0.125526, 0.407483, -1.984869
};

float DoorVerticies[] = 
{
	-1.133465, 1.184256, -1.986908,
	-1.133465, 0.004256, -1.986908,
	-1.633465, 0.004256, -1.986908,
	-1.633465, 1.184256, -1.986908
};

float Text_Coords[] = 
{
	0.000000, 0.000000,
	1.000000, 0.000000,
	1.000000, 1.000000,
	0.000000, 1.000000
};
	
	
/************************************************************************
  Collisions Checking and Updating
 ************************************************************************/

void checkInHoles(){
	for (int i=0; i< numOfBall; ++i){
		if (!balls[i]->isInHole)
		for (int j=0; j< 6;  ++j){
			Hole* hole = table.holes[j];
			if (hole->isBallInHole(balls[i])){
				hole->resToBallInHole(balls[i]);
				break;
			}								
		}
	}	
}

void checkColisions(){
	for (int i=0; i< numOfBall; ++i){
		for (int j=i+1; j< numOfBall; ++j){
			if ( !balls[i]->isInHole  && !balls[j]->isInHole && balls[i]->isBallHit(balls[j]) ){
				balls[i]->resToBallHit(balls[j]);
			}
		}
		if ( !balls[i]->isInHole ) table.resToBallHitTable(balls[i]);
	}
}

void updateBalls(){
	float dt= 0.1;
	for (int i=0; i< numOfBall; ++i){
		if (balls[i]->pos.y > 0.1)
			balls[i]->pos = balls[i]->pos +  balls[i]->vel*dt;				
		float stepLength = glm::length(balls[i]->vel*dt);
		float rotateAngle = stepLength*180/(M_PI*balls[i]->radius);		
		balls[i]->angle += rotateAngle;
	}		
}


/************************************************************************
  Objects Loading
 ************************************************************************/
		
void loadDoorAndImage()
{
	texture tex;
	string textureName = "BacHo.jpg";
	string texturePath = "resource/" + textureName;
	tex.image = SOIL_load_image(texturePath.c_str(), &(tex.width), &(tex.height), NULL, 0);
	textures.insert(textures.find(textureName), pair<string, texture>(textureName, tex));
	
	texture door;
	textureName = "door.jpg";
	texturePath = "resource/" + textureName;
	door.image = SOIL_load_image(texturePath.c_str(), &(door.width), &(door.height), NULL, 0);
	textures.insert(textures.find(textureName), pair<string, texture>(textureName, door));
}

void loadBalls()
{
	balls[0] = new Ball("resource/Ball3.obj", 0, &textures);	
	balls[0]->pos = glm::vec3(-0.1, 0.2774, 0.1);
	balls[0]->vel = glm::vec3(-0.05, 0, -0.05);
	balls[0]->acc = glm::vec3(-0.01, 0, -0.01);	
	balls[1] = new Ball("resource/Ball10.obj", 0, &textures);		
	balls[1]->pos = glm::vec3(0.22, 0.2774, 0.1);
	balls[1]->vel = glm::vec3(-0.05, 0, -0.05);
	balls[1]->acc = glm::vec3(-0.01, 0, -0.01);			
	balls[2] = new Ball("resource/Ball13.obj", 0, &textures);		
	balls[2]->pos = glm::vec3(0.4, 0.2774, -0.15);
	balls[2]->vel = glm::vec3(-0.05, 0, -0.05);
	balls[2]->acc = glm::vec3(-0.01, 0, -0.01);		
}


/************************************************************************
  Objects Drawing
 ************************************************************************/

void drawImage(float ImageVertices[], float Text_Coords[])
{
	glBegin(GL_QUADS);
	
	glTexCoord2f(Text_Coords[0], Text_Coords[1]);
	glVertex3f(ImageVertices[0], ImageVertices[1], ImageVertices[2]);
	
	glTexCoord2f(Text_Coords[2], Text_Coords[3]);
	glVertex3f(ImageVertices[3], ImageVertices[4], ImageVertices[5]);
	
	glTexCoord2d(Text_Coords[4], Text_Coords[5]);
	glVertex3f(ImageVertices[6], ImageVertices[7], ImageVertices[8]);
	
	glTexCoord2d(Text_Coords[6], Text_Coords[7]);
	glVertex3f(ImageVertices[9], ImageVertices[10], ImageVertices[11]);
	
	glEnd();
}

void drawDoorAndImage()
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);		
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glEnable(GL_TEXTURE_2D);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (textures.find("BacHo.jpg")->second).width, (textures.find("BacHo.jpg")->second).height,
				0, GL_RGB, GL_UNSIGNED_BYTE, (textures.find("BacHo.jpg")->second).image);
	drawImage(ImageVertices, Text_Coords);	 
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (textures.find("door.jpg")->second).width, (textures.find("door.jpg")->second).height,
				0, GL_RGB, GL_UNSIGNED_BYTE, (textures.find("door.jpg")->second).image);
	drawImage(DoorVerticies, Text_Coords);	 
}

void draw3DScence()
{	
	checkInHoles();	
	checkColisions();
	
	table.draw();		
	chairs.draw();	
	room.draw();
	drawDoorAndImage();
	for (int i=0; i< numOfBall; ++i) balls[i]->draw();		

	updateBalls();
}
 
void draw2DHUD(){
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0,win.width, win.height, 0.0, -1.0, 10.0);
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
	glDisable(GL_CULL_FACE);
	glClear(GL_DEPTH_BUFFER_BIT);    							

	float posx = win.width*0.05;
	float posy = win.height*0.1;
	float hudH= win.height*0.6;
	float hudW = win.width*0.05;
    
	if ( force <= 0 || force >= 100) fdir*=-1;
	force += 4*fdir;
    hudH = hudH*force/100;	
	glBegin(GL_POLYGON);  		  	
		glVertex2f(posx, posy);
		glVertex2f(posx+hudW, posy);				
		glVertex2f(posx+hudW, posy+hudH);
		glVertex2f(posx, posy+hudH);		
	glEnd();						
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);		
}
 
 
/************************************************************************
  GLUT Functions
 ************************************************************************/

void display() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt( position.x,position.y,position.z, -0.3,0.2774,0, 0,1,0);		
	draw3DScence();
	if (isHUDActive)
		draw2DHUD();
//	Sleep(10);		
	glutSwapBuffers();
}


void keyboard ( unsigned char key, int x, int y ) 
{
  switch ( key ) {
    case KEY_ESCAPE:        
      exit ( 0 );   
      break;      
    case 32 :
    	isHUDActive = true;
    	break;
    default:      
      break;
  }  
}
void keyUp( unsigned char key, int x, int y ){
  switch ( key ){
    case 32 :
    	isHUDActive = false;
    	force  = 0;
    	fdir = -1;
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
    glClearDepth( 1.0f );
    
    GLfloat amb_light[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
    
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, amb_light );
    
    glLightfv( GL_LIGHT1, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT1, GL_SPECULAR, specular );
    GLfloat light1_pos[] = {0, 1, 0, 1};
	glLightfv(GL_LIGHT1, GL_POSITION, light1_pos) ;
    glEnable( GL_LIGHT1 );
    
    glLightfv( GL_LIGHT2, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT2, GL_SPECULAR, specular );
    GLfloat light2_pos[] = {-1, 1, 0, 1};
	glLightfv(GL_LIGHT2, GL_POSITION, light2_pos) ;
    glEnable( GL_LIGHT2 );
    
    glLightfv( GL_LIGHT3, GL_DIFFUSE, diffuse );
    glLightfv( GL_LIGHT3, GL_SPECULAR, specular );
    GLfloat light3_pos[] = {1, 1, 0, 1};
	glLightfv(GL_LIGHT3, GL_POSITION, light3_pos) ;
    glEnable( GL_LIGHT3 );
    
    glEnable(GL_LIGHTING);
    
    glShadeModel( GL_SMOOTH );
    glDepthFunc( GL_LEQUAL );
    glEnable( GL_DEPTH_TEST );
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
	glutInit(&argc, argv);                                      // GLUT initialization
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );  // Display Mode
	glutInitWindowSize(win.width,win.height);					// set window size
	glutCreateWindow(win.title);								// create Window
	glutDisplayFunc(display);									// register Display Function
	glutIdleFunc( display );									// register Idle Function
    glutKeyboardFunc( keyboard );								// register Keyboard Handler
    glutKeyboardUpFunc(keyUp);
	initialize();

	table = Table("resource/pooltable.obj", 1, &textures);
	chairs = Model_OBJ("resource/chairs.obj", 0, &textures);
	room = Model_OBJ("resource/Room.obj", 1, &textures);
	loadDoorAndImage();
	loadBalls();
	
	/////////////////FOR CAMERA/////////////////////////////////////////////////////////////
  	glutMouseFunc(MouseEvent);
  	glutMotionFunc(mouseMotion);
  	/////////////////FOR CAMERA/////////////////////////////////////////////////////////////

	glutMainLoop();												// run GLUT mainloop
	return 0;
}
