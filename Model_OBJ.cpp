#include "Model_OBJ.h"

#define FLOATS_PER_VERTEX 3
#define VERTICES_PER_FACE 3
#define FLOATS_PER_TRIANGLE 9
#define FLOATS_PER_TEXTURE_COOR 2
#define FLOATS_PER_TEXTURES_COORS 6

using namespace std;


Model_OBJ::Model_OBJ()
{
	this->total_vertices_floats = 0;
	this->total_textures_coords_floats = 0;
	this->total_normal_vectors_floats = 0;
	this->hasTexture = false;
	this->total_objects = 0;
	this->total_materials = 0;
}

Model_OBJ::Model_OBJ(char* objFile, bool hasTexture, map<string, texture>* textures)
{
	this->total_vertices_floats = 0;
	this->total_textures_coords_floats = 0;
	this->total_normal_vectors_floats = 0;
	this->hasTexture = hasTexture;
	this->total_objects = 0;
	this->total_materials = 0;
	this->textures = textures;
	
	this->loadOBJ(objFile, hasTexture);
}

Model_OBJ::~Model_OBJ()
{
	
}

int Model_OBJ::loadMTL(char* filename)
{
	cout << "MTL is loading ...\n";
	
	int nMtl = -1;
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
				nMtl++;
				materials[nMtl].hasTexture = false;
				
				string l = "newmtl ";
				materials[nMtl].name = line.substr(l.size());
				cout << "newmtl " << materials[nMtl].name << endl;
			}
			else if (type.compare("Ns") == 0)
			{
				sscanf(line.c_str(), "Ns %f", &(materials[nMtl].Ns));
				cout << "Ns " << materials[nMtl].Ns << endl;
			}
			else if (type.compare("Ka") == 0)
			{
				sscanf(line.c_str(), "Ka %f %f %f", &(materials[nMtl].Ka[0]), &(materials[nMtl].Ka[1]), &(materials[nMtl].Ka[2]));
				cout << "Ka " << materials[nMtl].Ka[0] << " " << materials[nMtl].Ka[1] << " " << materials[nMtl].Ka[2] << endl;
			}
			else if (type.compare("Kd") == 0)
			{
				sscanf(line.c_str(), "Kd %f %f %f", &(materials[nMtl].Kd[0]), &(materials[nMtl].Kd[1]), &(materials[nMtl].Kd[2]));
				cout << "Kd " << materials[nMtl].Kd[0] << " " << materials[nMtl].Kd[1] << " " << materials[nMtl].Kd[2] << endl;
			}
			else if (type.compare("Ks") == 0)
			{
				sscanf(line.c_str(), "Ks %f %f %f", &(materials[nMtl].Ks[0]), &(materials[nMtl].Ks[1]), &(materials[nMtl].Ks[2]));
				cout << "Ks " << materials[nMtl].Ks[0] << " " << materials[nMtl].Ks[1] << " " << materials[nMtl].Ks[2] << endl;
			}
			else if (type.compare("Ni") == 0)
			{
				sscanf(line.c_str(), "Ni %f", &(materials[nMtl].Ni));
				cout << "Ni " << materials[nMtl].Ni << endl;
			}
			else if (type.compare("d ") == 0)
			{
				sscanf(line.c_str(), "d %f", &(materials[nMtl].d));
				cout << "d " << materials[nMtl].d << endl;
			}
			else if (type.compare("il") == 0)
			{
				sscanf(line.c_str(), "illum %d", &(materials[nMtl].illum));
				cout << "illum " << materials[nMtl].illum << endl;
			}
			else if (type.compare("ma") == 0)
			{
				materials[nMtl].hasTexture = true;
				
				string l = "map_Kd ";
				string textureName = line.substr(l.size());
				
				materials[nMtl].texture = textureName;
				cout << "map_Kd " << materials[nMtl].texture << endl;
				
				map<string, texture>::iterator it = textures->find(textureName);
				if (it == textures->end())
				{
					texture tex;
					string texturePath = "resource/" + textureName;
					tex.image = SOIL_load_image(texturePath.c_str(), &(tex.width), &(tex.height), NULL, 0);
					textures->insert(it, pair<string, texture>(textureName, tex));
				}
			}
		}	
		
		nMtl++;
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
		
		int obj_textures_floats = 0;
		int obj_normal_vectors_floats = 0;
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
				total_materials = loadMTL(cstr);
				cout << "Number of mtl: " << loadMTL(cstr) << endl;
			}
			
			// Create new object
			else if (type.compare("us") == 0)
			{
				obj_textures_floats = 0;
				obj_normal_vectors_floats = 0;
				obj = &objects[total_objects];
				string l = "usemtl ";
				string mtlName = line.substr(l.size());
				
				for (int i = 0; i < total_materials; i++)
				{
					if (mtlName.compare(string(materials[i].name)) == 0)
					{
						obj->material = &materials[i];
					}
				}
				
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
						obj->texts_coords[obj_textures_floats + tCounter] = vtBuffer[2*vtNumber[i]];
						obj->texts_coords[obj_textures_floats + tCounter + 1] = vtBuffer[2*vtNumber[i] + 1];
						tCounter += 2;
					} 
					
					obj_textures_floats += FLOATS_PER_TEXTURES_COORS;
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
					obj->norm_vectors[obj_normal_vectors_floats + tCounter] = vnBuffer[3*vnNumber[i] ];
					obj->norm_vectors[obj_normal_vectors_floats + tCounter + 1] = vnBuffer[3*vnNumber[i] + 1];
					obj->norm_vectors[obj_normal_vectors_floats + tCounter + 2] = vnBuffer[3*vnNumber[i] + 2];
					tCounter += 3;
				}
				obj_normal_vectors_floats += FLOATS_PER_TRIANGLE;
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
		material* mtl = objects[i].material;
		
		if (mtl->hasTexture)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		 	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			
			texture tex = textures->find(mtl->texture)->second;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex.width, tex.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex.image);
			
		 	glEnable(GL_TEXTURE_2D);
		 	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
		}
		
		float ambient[] = {mtl->Ka[0], mtl->Ka[1], mtl->Ka[2], 1.0};
		float diffuse[] = {mtl->Kd[0], mtl->Kd[1], mtl->Kd[2], 1.0};
		float specular[] = {mtl->Ks[0], mtl->Ks[1], mtl->Ks[2], 1.0};
		float shininess = mtl->Ns;

		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		glMaterialf(GL_FRONT, GL_SHININESS, shininess);
		
		glVertexPointer(3, GL_FLOAT, 0, objects[i].faces_triangles);				// Vertex Pointer to triangle array
		glTexCoordPointer(2, GL_FLOAT, 0, objects[i].texts_coords);
		glNormalPointer(GL_FLOAT, 0, objects[i].norm_vectors);						// Normal pointer to normal array
		glDrawArrays(GL_TRIANGLES, 0, objects[i].total_triangles_floats);		// Draw the triangles
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);					
	glDisableClientState(GL_NORMAL_ARRAY);				
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);	
}
