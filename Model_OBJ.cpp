#include "Model_OBJ.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <map>

#define FLOATS_PER_VERTEX 3
#define FLOATS_PER_TRIANGLE 9

using namespace std;

Model_OBJ::Model_OBJ()
{
	this->total_vertices_floats = 0;
	this->hasTexture = false;
	this->total_objects = 0;
}

Model_OBJ::~Model_OBJ()
{
	
}

int Model_OBJ::loadMTL(char* filename)
{
	cout << "MTL is loading ...\n";
	
	int nMtl = 0;
	string line;
	ifstream mtlFile (filename);
	
	if (!mtlFile.is_open())
	{
		cout << "Unable to open file!";
	}
	else
	{
		while (!mtlFile.eof())
		{
			getline(mtlFile, line);
			string type = line.substr(0,2);
			string mtlName;
			
			if (type.compare("ne") == 0)
			{
				string l = "newmtl ";
				mtlName = line.substr(l.size());
				cout << mtlName << endl;
				
				material mtl;
				materials.insert(pair<string, material>(mtlName, mtl));
				nMtl++;
			}
			else if (type.compare("Ns") == 0)
			{
				map<string, material>::iterator it = materials.find(mtlName);
				sscanf(line.c_str(), "Ns %f", &(it->second.Ns));
				cout << "Ns " << it->second.Ns << endl;
			}
			else if (type.compare("Ka") == 0)
			{
				map<string, material>::iterator it = materials.find(mtlName);
				sscanf(line.c_str(), "Ka %f %f %f", &(it->second.Ka[0]), &(it->second.Ka[1]), &(it->second.Ka[2]));
				cout << "Ka " << it->second.Ka[0] << " " << it->second.Ka[1] << " " << it->second.Ka[2] << endl;
			}
			else if (type.compare("Kd") == 0)
			{
				map<string, material>::iterator it = materials.find(mtlName);
				sscanf(line.c_str(), "Kd %f %f %f", &(it->second.Kd[0]), &(it->second.Kd[1]), &(it->second.Kd[2]));
				cout << "Kd " << it->second.Kd[0] << " " << it->second.Kd[1] << " " << it->second.Kd[2] << endl;
			}
			else if (type.compare("Ks") == 0)
			{
				map<string, material>::iterator it = materials.find(mtlName);
				sscanf(line.c_str(), "Ks %f %f %f", &(it->second.Ks[0]), &(it->second.Ks[1]), &(it->second.Ks[2]));
				cout << "Ks " << it->second.Ks[0] << " " << it->second.Ks[1] << " " << it->second.Ks[2] << endl;
			}
			else if (type.compare("Ni") == 0)
			{
				map<string, material>::iterator it = materials.find(mtlName);
				sscanf(line.c_str(), "Ni %f", &(it->second.Ni));
				cout << "Ni " << it->second.Ni << endl;
			}
			else if (type.compare("d ") == 0)
			{
				map<string, material>::iterator it = materials.find(mtlName);
				sscanf(line.c_str(), "d %f", &(it->second.d));
				cout << "d " << it->second.d << endl;
			}
			else if (type.compare("il") == 0)
			{
				map<string, material>::iterator it = materials.find(mtlName);
				sscanf(line.c_str(), "illum %d", &(it->second.illum));
				cout << "illum " << it->second.illum << endl;
			}
			else if (type.compare("ma") == 0)
			{
				string l = "map_Kd ";
				string texture = line.substr(l.size());
				map<string, material>::iterator it = materials.find(mtlName);
				it->second.texture = texture.c_str();
				cout << "map_Kd " << it->second.texture << endl;
			}
		}	
		
		mtlFile.close();
	}
	
	return nMtl;
}

void Model_OBJ::loadOBJ(char* filename, bool hasTexture)
{
	this->hasTexture = hasTexture;
	
	string line;
	ifstream objFile (filename);
	
	queue<object> objs;
	
	if (!objFile.is_open())
	{
		cout << "Unable to open file!";
	}
	else
	{
		objFile.seekg (0, ios::end);										
		long fileSize = objFile.tellg();
		objFile.seekg (0, ios::beg);
		
		vertexBuffer = (float*) malloc (fileSize);
		vtBuffer = (float*) malloc (fileSize);
		vnBuffer = (float*) malloc (fileSize);
		
		while (!objFile.eof())
		{
			getline(objFile, line);
			string type = line.substr(0,2);
			
			//	Load material file
			if (type.compare("mt") == 0)
			{
				string l = "mtllib ";
				string mtlFile = line.substr(l.size());
				mtlFile = "resource/" + mtlFile;
				
				const char* cstr = mtlFile.c_str();
				cout << "Number of mtl: " << loadMTL(cstr) << endl;
			}
			
			// Create new object
			else if (type.compare("us") == 0)
			{
				object obj;
				string l = "usemtl ";
				
				objects[total_objects].material = (line.substr(l.size())).c_str();
				objects[total_objects].total_triangles_floats = 0;
				objects[total_objects].faces_triangles = (float*) malloc (fileSize);
				objects[total_objects].texts_coords = (float*) malloc (fileSize);
				objects[total_objects].norm_vectors = (float*) malloc (fileSize);
				
				total_objects++;		
			}
			
			//	Read vertex
			else if (type.compare("v ") == 0)
			{
				float vertices[3];
				sscanf(line.c_str(), "v %f %f %f", 
					&vertexBuffer[total_vertices_floats], 
					&vertexBuffer[total_vertices_floats + 1], 
					&vertexBuffer[total_vertices_floats + 2]);
				
//				cout << vertices[0] << " " << vertices[1] << " " << vertices[2] << endl;
				
//				vertexBuffer[total_vertices_floats] = vertices[0];
//				vertexBuffer[total_vertices_floats + 1] = vertices[1];
//				vertexBuffer[total_vertices_floats + 2] = vertices[2];
//					
				total_vertices_floats += FLOATS_PER_VERTEX;			
			}
		}
		
//		while (!objs.empty())
//		{
//			this->objects.push(objs.front());
//			objs.pop();	
//		}
		
		objFile.close();
	}
}

