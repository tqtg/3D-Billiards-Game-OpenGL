#include <string>
#include <queue>
#include <map>

using namespace std;

/************************************************************************
  Material
 ************************************************************************/
 
typedef struct {
    float Ns;
    float Ka[3];
    float Kd[3];
    float Ks[3];
    float Ni;
    float d;
    int illum;
    char *texture;
} material;

class Model_OBJ
{
private:
	map<string, material> materials;
public:

public:		
	Model_OBJ();
	~Model_OBJ();
	
	void loadMTL(char *filename);
};
