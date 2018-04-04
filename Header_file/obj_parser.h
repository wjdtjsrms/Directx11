#ifndef _OBJ_PARSER_H_
#define _OBJ_PARSER_H_

#include <iostream>
#include <fstream>
#include <D3D11.h>
#include <xnamath.h>

using namespace std;

typedef struct
{
	float x, y, z;
}VertexType;

typedef struct
{
	int vIndex1, vIndex2, vIndex3;
	int tIndex1, tIndex2, tIndex3;
	int nIndex1, nIndex2, nIndex3;
}FaceType;

class  Obj_loader
{
public:
	 Obj_loader();
	 Obj_loader(const Obj_loader&);
	~ Obj_loader();

	bool Initialize(char *);
	bool GetModel(char *);
	bool ReadFile(char *);
	bool LoadData(char *);


private:
	bool GetModelFilename(char*);
	bool ReadFileCounts(char*, int&, int&, int&, int&);
	bool LoadDataStructures(char*,  int, int, int, int);



private:
	VertexType *vertices, *texcoords, *normals;
	FaceType *faces;
	int vertexCount, textureCount, normalCount, faceCount;


};

#endif