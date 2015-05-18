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


/************************************************************************
	Camera
 ************************************************************************/
const int numOfBall = 4;
Ball* balls[numOfBall];
float PI = 3.14;
glm::vec3 position(1, 1, 1);
float distanceFromObject = 1.792;
float angleAroundObject = 0;
float pitch = -40;
float yaw = 0;
float roll;
float originalPitch;
int oldMouseX, oldMouseY;
bool mouseDown, isLeftClick, isRightClick, flag;

float calculateHorizontalDistance(){
	return distanceFromObject*cos(pitch*PI/180);
}
float calculateVerticalDistance(){
	return distanceFromObject*sin(pitch*PI/180);
}

void calculateCameraPosition(float horizDistance, float verticDistance){
	float theta = 0 + angleAroundObject;
	float offsetX = horizDistance * sin(theta*PI/180);
	float offsetZ = horizDistance * cos(theta*PI/180);
	position.x = balls[0]->pos[0] - offsetX;
	position.z = balls[0]->pos[1] - offsetZ;
	position.y = balls[0]->pos[1] - verticDistance;
}

void calculateZoom(float command){
	//Command is 1 or -1 which equivalent to zoom in and zoom out
	distanceFromObject += command * 0.1f;
}

void calculatePitch(float MouseDY){
	//ensure that right button is down
	originalPitch = pitch;
	pitch -= MouseDY * 0.08f;
	if (!(abs(pitch)>1 && abs(pitch)<40) | pitch>=0){
		pitch = originalPitch;
	}
//	cout << pitch << endl;
}

void calculateAngleAroundObject(float MouseDX){
	//ensure that left button is down
	float angleChange = MouseDX * 0.08f;
	angleAroundObject -= angleChange;
}

void move(float command, float MouseDY, float MouseDX){
	calculateZoom(command);
	calculatePitch(MouseDY);
	calculateAngleAroundObject(MouseDX);
	float horizontalDistance = calculateHorizontalDistance();
	float verticalDistance = calculateVerticalDistance();
	calculateCameraPosition(horizontalDistance, verticalDistance);
	yaw = 180 - angleAroundObject;
}


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
		if (DY>0 && distanceFromObject < 2.4) move(1, 0, 0);
		if (DY<0 && distanceFromObject > 1.5) move(-1, 0, 0);
		oldMouseY = y;
	}	
}

void keyboardCam(){
	
}
/************************************************************************
	Camera
 ************************************************************************/


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
Table table;
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
	flag = false;
	if (balls[0]->isInHole){
		for (int i=0; i< numOfBall; ++i){
			if (glm::length(balls[i]->vel) != 0){
				flag = true;
				break;
			}
		}
		if ( flag == false){
			Sleep (200);
			balls[0]->pos = glm::vec3(-0.3, table.heigh + balls[0]->radius, 0);
			balls[0]->vel = glm::vec3(0, 0, 0);	
			balls[0]->isInHole = false;
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
	float dt= 0.05;
	for (int i=0; i< numOfBall; ++i){										
		float stepLength = glm::length(balls[i]->vel)*dt;
		float rotateAngle = stepLength*180/(M_PI*balls[i]->radius);				
		balls[i]->angle += rotateAngle;
			
		if ( glm::length(balls[i]->vel) < 0.01) 
			balls[i]->vel = glm::vec3(0,0,0); 
		else {
			glm::vec3 acc = glm::normalize(balls[i]->vel)*-0.08f;							
			balls[i]->vel = balls[i]->vel + acc*dt;
		}
				
		if (balls[i]->pos.y > 0.22)
			balls[i]->pos = balls[i]->pos +  balls[i]->vel*dt;											
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
	balls[0] = new Ball("resource/Ball0.obj", 0, &textures);	
	balls[0]->pos = glm::vec3(-0.3, table.heigh + balls[0]->radius, 0);
	balls[0]->vel = glm::vec3(0, 0, 0);	
	balls[1] = new Ball("resource/Ball10.obj", 0, &textures);		
	balls[1]->pos = glm::vec3(0.2, table.heigh + balls[1]->radius, 0.0);
	balls[1]->vel = glm::vec3(0, 0, 0);	
	balls[2] = new Ball("resource/Ball13.obj", 0, &textures);		
	balls[2]->pos = glm::vec3(0.3, table.heigh + balls[2]->radius, 0.1);
	balls[2]->vel = glm::vec3(0, 0, 0);	
	balls[3] = new Ball("resource/Ball3.obj", 0, &textures);		
	balls[3]->pos = glm::vec3(0.3, table.heigh + balls[3]->radius, -0.1);
	balls[3]->vel = glm::vec3(0, 0, 0);	
	
	move(0, 0, 0);
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

void drawStick(){
	if ( balls[0]->isInHole) return;
	for ( int i=0; i< numOfBall; ++i)
		if (glm::length(balls[0]->vel) > 0)	 return;
	glm::vec3 stick = balls[0]->pos - glm::vec3(position.x, table.heigh, position.z);	
	stick = glm::normalize(stick)*0.3f;
	
	glLineStipple(3, 0xAAAA);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glVertex3f(balls[0]->pos.x, balls[0]->pos.y, balls[0]->pos.z);
		glVertex3f(balls[0]->pos.x + stick.x, balls[0]->pos.y + stick.y + 5, balls[0]->pos.z + stick.z);
	glEnd();
}

void draw3DScence()
{	
	checkInHoles();	
	checkColisions();
	
	table.draw();
	chairs.draw();	
	room.draw();
	drawDoorAndImage();
	drawStick();	
	updateBalls();	
	for (int i=0; i< numOfBall; ++i) balls[i]->draw();			
}
 
void draw2DHUD(){
	if (!isHUDActive || balls[0]->isInHole) return;	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0,win.width, win.height, 0.0, -1.0, 10.0);
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
	glDisable(GL_CULL_FACE);
	glClear(GL_DEPTH_BUFFER_BIT);    							

	float posx = win.width*0.93;
	float posy = win.height*0.8;
	float hudH= win.height*0.6;
	float hudW = win.width*0.03;
	
	glDisable(GL_LIGHTING);
    glColor3f(0.5, 0.5, 0.5);
	glLineWidth(2.0);		 
	glBegin(GL_LINE_STRIP);   	
		glVertex2f(posx, posy);
		glVertex2f(posx+hudW, posy);				
		glVertex2f(posx+hudW, posy-hudH);
		glVertex2f(posx, posy-hudH);	
		glVertex2f(posx, posy);	
	glEnd();
    
	if ( force <= 0 || force >= 100) fdir*=-1;
	force += 4*fdir;
    hudH = hudH*force/100;
	glBegin(GL_POLYGON);  		  	
		glVertex2f(posx, posy);
		glVertex2f(posx+hudW, posy);				
		glVertex2f(posx+hudW, posy-hudH);
		glVertex2f(posx, posy-hudH);		
	glEnd();	
	glEnable(GL_LIGHTING);
						
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
	gluLookAt( position.x,position.y,position.z, balls[0]->pos[0], balls[0]->pos[1], balls[0]->pos[2], 0,1,0);	

	draw3DScence();
	if (isHUDActive)
		draw2DHUD();
	Sleep(10);		
	glutSwapBuffers();
}
void strikeBall(){
	glm::vec2 cueVel2D = glm::vec2(balls[0]->pos.x,balls[0]->pos.z) - glm::vec2(position.x,position.z);	
	cueVel2D = glm::normalize(cueVel2D)*(float)(force/100);	
	glm::vec3 cueVel3D = glm::vec3(cueVel2D.x,0,cueVel2D.y);
	balls[0]->vel = cueVel3D;
}	


void keyboard ( unsigned char key, int x, int y ) 
{
  switch ( key ) {
    case KEY_ESCAPE:        
      exit ( 0 );   
      break;      
    case 32 :
    	if (!balls[0]->isInHole)
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
    	strikeBall();
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
