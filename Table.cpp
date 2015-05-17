#include "Table.h"

Table::Table(char* objFile, bool hasTexture, map<string, texture>* textures):Model_OBJ(objFile, hasTexture, textures){
	this->left = -0.42;
	this->right = 0.42;
	this->top = -0.17;
	this->bottom = 0.18;
	this->heigh = 0.2574;
}

bool Table::isBallHitLeftBorder(Ball *ball){
	if ( ball->pos.x <= (this->left + ball->radius) ) 
		return true;
	else 
		return false;
}


bool Table::isBallHitRightBorder(Ball *ball){
	if ( ball->pos.x >= (this->right - ball->radius) )
		return true;
	else
		return false;
}

bool Table::isBallHitTopBorder(Ball *ball){
	if ( ball->pos.z <= (this->top + ball->radius) )
		return true;
	else 
		return false;
}

bool Table::isBallHitBottomBorder(Ball *ball){
	if ( ball->pos.z >= (this->bottom - ball->radius) )
		return true;
	else 
		return false;	
}

void Table::resToBallHitTable(Ball *ball){
	if ( isBallHitLeftBorder(ball) ){
		ball->vel.x = -ball->vel.x;	
		ball->pos.x = left + ball->radius + 0.01;
	} else 
	if ( isBallHitRightBorder(ball) ){
		ball->vel.x = -ball->vel.x;	
		ball->pos.x = right - ball->radius - 0.01;
	}		
				
	if ( isBallHitTopBorder(ball)){
		ball->vel.z = -ball->vel.z;								
		ball->pos.z = top + ball->radius + 0.01;		
	} else 
	if ( isBallHitBottomBorder(ball)){
		ball->vel.z = -ball->vel.z;								
		ball->pos.z = bottom - ball->radius - 0.01;				
	}
		
}

