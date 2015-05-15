#include <string>
#include <queue>
#include <map>

#define MAX_OBJ 50

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
    char* texture;
} material;


/************************************************************************
  Object
 ************************************************************************/
 
typedef struct {
	char* material;
	float* faces_triangles;
	float* texts_coords;
	float* norm_vectors;
	long total_triangles_floats;
} object;


/************************************************************************
  Model
 ************************************************************************/

class Model_OBJ
{
private:
	map<string, material> materials;
	object objects[MAX_OBJ];
	
	float* vertexBuffer;
	float* vtBuffer;
	float* vnBuffer;
	
	bool hasTexture;
	long total_vertices_floats;
	int total_objects;

public:		
	Model_OBJ();
	~Model_OBJ();
	
	int loadMTL(char *filename);
	void loadOBJ(char *filename, bool hasTexture);
};
