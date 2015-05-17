#include "Hole.h"
Hole::Hole(glm::vec3 pos,float radius){
	this->pos = pos;
	this->radius = radius;	
}
bool Hole::isBallInHole(Ball *ball){
	glm::vec3 distance = this->pos - ball->pos;
	float length = glm::length(distance);
	if ( length <= (this->radius + ball->radius+ 0.01) )
		return true;
	else 
		return false;	
}

void Hole::resToBallInHole(Ball *ball){
	ball->pos = this->pos;
	ball->isInHole = true;
	ball->vel = glm::vec3(0,-0.05,0);
}
