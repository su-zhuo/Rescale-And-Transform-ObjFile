#ifndef __OBJMTL_H__
#define __OBJMTL_H__

#include <vector>
#include <cuda.h>
#include <cuda_runtime.h>
#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>


struct TriangleFace
{
	long int v[3]; // vertex indices
	long int vt[3]; // texture indices
};

struct Objmtl
{

	char mtllibName[256];
	std::vector<float3> verts;
	std::vector<float2> texture;
	std::vector<TriangleFace> faces;
	float3 massCenter;
	float3 bbx[2];
};

void read_objmtl(const std::string filename, Objmtl &mesh);
void Store_objmtl(std::string filename, Objmtl &mesh);

void read_objmtl(const std::string filename, Objmtl &mesh)
{
	std::ifstream in(filename.c_str());
	if (!in.good())
	{
		std::cout << "error: reading obj :" << filename << std::endl;
		exit(0);
	}

	char buffer[256], str[255], str1[255];

	while (!in.getline(buffer, 255).eof())
	{
		buffer[255] = '\0';
		sscanf_s(buffer, "%s", str, 255);

		//read vertex
		if (buffer[0] == 'v' && (buffer[1] == ' ' || buffer[1] == 32))
		{
			float3 v;
			if (sscanf(buffer, "v %f %f %f", &v.x, &v.y, &v.z))
			{
				mesh.verts.push_back(v);
			}
			else
			{
				std::cout << "error: vertex not in wanted format" << std::endl;
				exit(-1);
			}
		}
		//read texture
		if (buffer[0] == 'v' && buffer[1] == 't' && (buffer[2] == ' ' || buffer[2] == 32))
		{
			float2 vt;
			if (sscanf(buffer, "vt %f %f", &vt.x, &vt.y))
			{
				mesh.texture.push_back(vt);
			}
			else
			{
				std::cout << "error: texture not in wanted format" << std::endl;
				exit(-1);
			}
		}
		//read face
		if (buffer[0] == 'f' && (buffer[1] == ' ' || buffer[1] == 32))
		{
			TriangleFace face;
			if (sscanf(buffer, "f %d\/%d %d\/%d %d\/%d", &face.v[0], &face.vt[0], &face.v[1], &face.vt[1], &face.v[2], &face.vt[2]))
			{
				mesh.faces.push_back(face);
			}
			else
			{
				std::cout << "error: face not in wanted format" << std::endl;
				exit(-1);
			}
		}

		//read mtllibName
		if (buffer[0] == 'm' && buffer[1] == 't')
		{
			std::cout << buffer << std::endl;
			for (int i = 0; i < 256; i++)
				mesh.mtllibName[i] = buffer[i];
		}

	}


	float3 mC = { 0,0,0 };
	float3 bx[2] = { mesh.verts[0], mesh.verts[1] };
	for (int i = 0; i < mesh.verts.size(); i++)
	{
		mC.x += mesh.verts[i].x;
		mC.y += mesh.verts[i].y;
		mC.z += mesh.verts[i].z;
		if (bx[0].x > mesh.verts[i].x)
			bx[0].x = mesh.verts[i].x;
		if (bx[0].y > mesh.verts[i].y)
			bx[0].y = mesh.verts[i].y;
		if (bx[0].z > mesh.verts[i].z)
			bx[0].z = mesh.verts[i].z;
		if (bx[1].x < mesh.verts[i].x)
			bx[1].x = mesh.verts[i].x;
		if (bx[1].y < mesh.verts[i].y)
			bx[1].y = mesh.verts[i].y;
		if (bx[1].z < mesh.verts[i].z)
			bx[1].z = mesh.verts[i].z;
	}
	mC.x /= mesh.verts.size();
	mC.y /= mesh.verts.size();
	mC.z /= mesh.verts.size();
	mesh.massCenter = mC;
	mesh.bbx[0] = bx[0];
	mesh.bbx[1] = bx[1];
}


void Store_objmtl(std::string filename, Objmtl &mesh)
{
	try
	{
		std::ofstream of(filename);
		//write others
		of << mesh.mtllibName << std::endl;
		of << "usemtl material_0" << std::endl;
		//write vertex
		int vnum = (int)mesh.verts.size();
		for (int i = 0; i < vnum; i++)
		{
			of << "v " << mesh.verts[i].x << " " << mesh.verts[i].y << " " << mesh.verts[i].z << std::endl;
		}
		//write texture
		for (int i = 0; i < mesh.texture.size(); i++)
		{
			of << "vt " << mesh.texture[i].x << " " << mesh.texture[i].y << std::endl;
		}
		//write face
		for (int i = 0; i < mesh.faces.size(); i++)
		{
			of << "f " << mesh.faces[i].v[0] << "/" << mesh.faces[i].vt[0] << " " << mesh.faces[i].v[1] << "/" <<
				mesh.faces[i].vt[1] << " " << mesh.faces[i].v[2] << "/" << mesh.faces[i].vt[2] << std::endl;
		}
		of.close();
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		exit(-1);
	}
	printf("store obj file finished!\n");
}


#endif