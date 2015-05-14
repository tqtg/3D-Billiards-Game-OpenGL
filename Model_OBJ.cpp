#include "Model_OBJ.h"
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

Model_OBJ::Model_OBJ()
{
	
}

Model_OBJ::~Model_OBJ()
{
	
}

void Model_OBJ::loadMTL(char *filename)
{
	cout << "MTL is loading ...\n";
	
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
	}
}
