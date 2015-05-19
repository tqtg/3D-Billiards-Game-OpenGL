#include "Ball.h"
Ball::Ball(char* objFile, bool hasTexture, map<string, texture>* textures):Model_OBJ(objFile, hasTexture, textures){
	this->radius = 0.02;
	this->mass = 1;		
	this->orientation = glm::vec3(0,0,0);
}

bool Ball::isBallHit(Ball* ball){
	glm::vec3 distance = this->pos - ball->pos;
	float length = glm::length(distance);
	float sumradius = this->radius + ball->radius;	
	if (length < sumradius) 
		return true;
	else 
		return false;				
}
void Ball::resToBallHit(Ball *ball){	
	float tmp;
	glm::vec3 normal, tangential, v1, v2, v1n, v1t, v2n, v2t;
	
	normal = ball->pos - this->pos;	
	tmp = this->radius  - glm::length(normal)/2;	
	normal = glm::normalize(normal);
	
	v1 = this->vel;	
	v1n = normal*glm::dot(normal,this->vel);
	v1t = v1 - v1n;	
	
	normal = normal * -1.0f;
	v2 = ball->vel;	
	v2n = normal*glm::dot(normal,v2);
	v2t = v2 - v2n;		
	
	this->vel =  v2n + v1t;	
	ball->vel =  v1n + v2t;		
	
	this->pos = this->pos + normal*tmp;
	ball->pos = ball->pos - normal*tmp;
	
}
void Ball::draw(){	
	if (glm::length(vel) > 0)	orientation = vel;	
	if ( isInHole )	 orientation = glm::vec3(0,0,0);
	glPushMatrix();				
		glTranslatef(pos.x, pos.y , pos.z);																	
		if (angle >= 360) angle =0;		
		if (!isInHole && glm::length(orientation) != 0 ){
			glRotatef(-angle,-orientation.z,0,orientation.x);					
		}								
		Model_OBJ::draw();
	glPopMatrix();
}

