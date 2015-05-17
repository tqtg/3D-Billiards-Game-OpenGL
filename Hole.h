#include "Ball.h"
class Hole{
	public:
		Hole(glm::vec3 pos, float radiuss);
		bool isBallInHole(Ball *ball);		
		void resToBallInHole(Ball *ball);		
	public:
		glm::vec3 pos;
		float radius;		
};
