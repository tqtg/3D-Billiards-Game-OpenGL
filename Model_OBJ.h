#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <map>
#include <GL/glut.h>
#include "lib/SOIL.h"

#define MAX_MTL 10
#define MAX_OBJ 50

using namespace std;

/************************************************************************
  Material
 ************************************************************************/
 
typedef struct {
	string name;
    float Ns;
    float Ka[3];
    float Kd[3];
    float Ks[3];
    float Ni;
    float d;
    int illum;
	bool hasTexture;
    string texture;
} material;


/************************************************************************
  Object
 ************************************************************************/
 
typedef struct {
	material* material;
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
	
	object objects[MAX_OBJ];
	
	float* vertexBuffer;
	float* vtBuffer;
	float* vnBuffer;
	
	bool hasTexture;
	long total_vertices_floats;
	long total_textures_coords_floats;
	long total_normal_vectors_floats;
	int total_objects;
public:
	material materials[MAX_MTL];
	int total_materials;

public:		
	Model_OBJ();
	Model_OBJ(char* objFile, bool hasTexture);
	~Model_OBJ();
	
	int loadMTL(char *filename);
	void loadOBJ(char *filename, bool hasTexture);
	void draw();
};
