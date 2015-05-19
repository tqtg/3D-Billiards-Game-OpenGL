#include "Model_OBJ.h"
#include "lib/glm/glm.hpp"
#include "lib/glm/gtx/vector_angle.hpp"
class Ball: public Model_OBJ{
	public: 
		Ball(){}
		Ball(char* objFile, bool hasTexture, map<string, texture>* textures);
		bool isBallHit(Ball *ball);		
		void resToBallHit(Ball *ball);
		void draw();
	public:						
		float radius;
		float mass;
		glm::vec3 pos;
		glm::vec3 vel;		
		glm::vec3 orientation;		
		float angle = 0;			
		bool isInHole = false;				
};
