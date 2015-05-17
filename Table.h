#include "Ball.h"
class Table: public Model_OBJ{
	public:
		Table(){}
		Table(char* objFile, bool hasTexture, map<string, texture>* textures);
		bool isBallHitLeftBorder(Ball *ball);
		bool isBallHitRightBorder(Ball *ball);		
		bool isBallHitTopBorder(Ball *ball);				
		bool isBallHitBottomBorder(Ball *ball);						
		void resToBallHitTable(Ball *ball);
	public:
		float left, right, top , bottom;
		float heigh;
};
