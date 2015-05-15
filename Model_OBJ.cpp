#include "Model_OBJ.h"

#define FLOATS_PER_VERTEX 3
#define VERTICES_PER_FACE 3
#define FLOATS_PER_TRIANGLE 9
#define FLOATS_PER_TEXTURE_COOR 6

using namespace std;

Model_OBJ::Model_OBJ()
{
	this->total_vertices_floats = 0;
	this->total_textures_coords_floats = 0;
	this->total_normal_vectors_floats = 0;
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
				cout << "newmtl " << mtlName << endl;
				
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
		
		int obj_textures_index = 0;
		int obj_normal_vectors_index = 0;
		object* obj;
		
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
				obj_textures_index = 0;
				obj_normal_vectors_index = 0;
				obj = &objects[total_objects];
				string l = "usemtl ";
				
				objects[total_objects].material = (line.substr(l.size())).c_str();
				obj->total_triangles_floats = 0;
				obj->faces_triangles = (float*) malloc (fileSize);
				obj->norm_vectors = (float*) malloc (fileSize);
				if (hasTexture)
				{
					obj->texts_coords = (float*) malloc (fileSize);
				}
				
				total_objects++;		
			}
			
			//	Read vertex
			else if (type.compare("v ") == 0)
			{
				sscanf(line.c_str(), "v %f %f %f", 
					&vertexBuffer[total_vertices_floats], 
					&vertexBuffer[total_vertices_floats + 1], 
					&vertexBuffer[total_vertices_floats + 2]);

				total_vertices_floats += FLOATS_PER_VERTEX;		
			}
			
			//	Read texture coordinates
			else if (type.compare("vt") == 0)
			{
				sscanf(line.c_str(), "vt %f %f",
					&vtBuffer[total_textures_coords_floats],
					&vtBuffer[total_textures_coords_floats + 1]);
				
				total_textures_coords_floats += FLOATS_PER_TEXTURE_COOR;
			}
			
			//	Read normal vectors
			else if (type.compare("vn") == 0)
			{
				sscanf(line.c_str(), "vn %f %f %f",
					&vnBuffer[total_normal_vectors_floats],
					&vnBuffer[total_normal_vectors_floats + 1],
					&vnBuffer[total_normal_vectors_floats + 2]);
				
				total_normal_vectors_floats += FLOATS_PER_VERTEX;
			}
			
			//	Read faces
			else if (type.compare("f ") == 0)
			{
				int tCounter = 0;
				unsigned int vertexNumber[3], vtNumber[3], vnNumber[3];
				
				if (hasTexture)
				{
					sscanf(line.c_str(),"f %d/%d/%d %d/%d/%d %d/%d/%d",
					&vertexNumber[0], &vtNumber[0], &vnNumber[0],
					&vertexNumber[1], &vtNumber[1], &vnNumber[1],
					&vertexNumber[2], &vtNumber[2], &vnNumber[2] );
					
					vtNumber[0] -= 1;										// OBJ file starts counting from 1
					vtNumber[1] -= 1;										// OBJ file starts counting from 1
					vtNumber[2] -= 1;										// OBJ file starts counting from 1
					
					tCounter = 0;
					for (int i = 0; i < VERTICES_PER_FACE; i++) 
					{
						obj->texts_coords[obj_textures_index + tCounter] = vtBuffer[2*vtNumber[i]];
						obj->texts_coords[obj_textures_index + tCounter + 1] = vtBuffer[2*vtNumber[i] + 1];
						tCounter += 2;
					} 
					
					obj_textures_index += FLOATS_PER_TEXTURE_COOR;
				}
				else
				{
					sscanf(line.c_str(),"f %d//%d %d//%d %d//%d",
					&vertexNumber[0], &vnNumber[0],			
					&vertexNumber[1], &vnNumber[1],			
					&vertexNumber[2], &vnNumber[2]);		
				}
				
				vertexNumber[0] -= 1;										// OBJ file starts counting from 1
				vertexNumber[1] -= 1;										// OBJ file starts counting from 1
				vertexNumber[2] -= 1;										// OBJ file starts counting from 1
				
				vnNumber[0] -= 1;										// OBJ file starts counting from 1
				vnNumber[1] -= 1;										// OBJ file starts counting from 1
				vnNumber[2] -= 1;										// OBJ file starts counting from 1
				
				//	object faces triangles
				tCounter = 0;
				for (int i = 0; i < VERTICES_PER_FACE; i++)
				{
					obj->faces_triangles[obj->total_triangles_floats + tCounter] = vertexBuffer[3*vertexNumber[i] ];
					obj->faces_triangles[obj->total_triangles_floats + tCounter + 1] = vertexBuffer[3*vertexNumber[i] + 1];
					obj->faces_triangles[obj->total_triangles_floats + tCounter + 2] = vertexBuffer[3*vertexNumber[i] + 2];
					tCounter += 3;
				}
				
				obj->total_triangles_floats += FLOATS_PER_TRIANGLE;
				
				//	object normal vectors
				tCounter = 0;
				for (int i = 0; i < VERTICES_PER_FACE; i++)
				{
					obj->norm_vectors[obj_normal_vectors_index + tCounter] = vertexBuffer[3*vnNumber[i] ];
					obj->norm_vectors[obj_normal_vectors_index + tCounter + 1] = vertexBuffer[3*vnNumber[i] + 1];
					obj->norm_vectors[obj_normal_vectors_index + tCounter + 2] = vertexBuffer[3*vnNumber[i] + 2];
					tCounter += 3;
				}
				
				obj_normal_vectors_index += FLOATS_PER_TRIANGLE;
			}
		}
		
		objFile.close();
		cout << "Total objects: " << total_objects << endl;
	}
}


void Model_OBJ::draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
 	glEnableClientState(GL_NORMAL_ARRAY);						
 	glEnableClientState (GL_TEXTURE_COORD_ARRAY);			
	
	for (int i = 0; i < total_objects; i++)
	{
//		string name(objects[i].material);
////		material* mtl = &(materials.find("04_-_Default")->second);
//		
//		if (!hasTexture)
//		{
//			cout << name << endl;
//		}
		
		glVertexPointer(3, GL_FLOAT, 0, objects[i].faces_triangles);				// Vertex Pointer to triangle array
		glTexCoordPointer(2, GL_FLOAT, 0, objects[i].texts_coords);
		glNormalPointer(GL_FLOAT, 0, objects[i].norm_vectors);						// Normal pointer to normal array
		glDrawArrays(GL_TRIANGLES, 0, objects[i].total_triangles_floats);		// Draw the triangles
	}

	glDisableClientState(GL_VERTEX_ARRAY);					
	glDisableClientState(GL_NORMAL_ARRAY);				
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);	
}
