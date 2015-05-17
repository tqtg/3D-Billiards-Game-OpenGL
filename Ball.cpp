#include "Ball.h"
Ball::Ball(char* objFile, bool hasTexture, map<string, texture>* textures):Model_OBJ(objFile, hasTexture, textures){
	this->radius = 0.02;
	this->mass = 1;			
	memset(col,0,sizeof(col));
}

bool Ball::isBallHit(Ball* ball){
	glm::vec3 distance = this->pos - ball->pos;
	float length = glm::length(distance);
	float sumradius = this->radius + ball->radius;	
	if (length <= sumradius) 
		return true;
	else 
		return false;				
}
void Ball::resToBallHit(Ball *ball){
	glm::vec3 U1x, U1y, U2x, U2y, V1x, V1y, V2x, V2y;
	float m1, m2, x1, x2;
	glm::vec3 v1temp, v1, v2, v1x, v2x, v1y, v2y, x;
	x = this->pos - ball->pos;	
	x = glm::normalize(x);
	v1 = this->vel;
	x1 = glm::dot(x,v1);
	v1x = x*x1;
	v1y = v1 - v1x;
	m1 = this->mass;
	
	x = x * -1.0f;
	v2 = ball->vel;
	x2 = glm::dot(x,v2);
	v2x = x*x2;
	v2y = v2 - v2x;
	m2 = ball->mass;
	
	float tmp1 = (m1-m2)/(m1+m2);
	float tmp2 = (2*m2)/(m1+m2);
	float tmp3 = (2*m1)/(m1+m2);
	float tmp4 = (m2-m1)/(m1+m2);
	this->vel = v1x*tmp1 + v2x*tmp2 + v1y;	
	ball->vel = v1x*tmp3 + v2x*tmp4 + v2y;		
	
	this->pos = this->pos - x*this->radius;
	ball->pos = ball->pos + x*this->radius;	
	
}
void Ball::draw(){
	glPushMatrix();								
		cout << angle << endl;
		glTranslatef(pos.x, pos.y , pos.z);								
		glRotatef(-angle,-vel.z,0,vel.x);		
		Model_OBJ::draw();
	glPopMatrix();
}

