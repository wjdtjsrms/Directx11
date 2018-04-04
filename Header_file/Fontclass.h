#ifndef _FONTCLASS_H_
#define _FONTCLASS_H_

#include <D3D11.h>
#include <xnamath.h>
#include <fstream>

using namespace std;

#include "Textureclass.h"

class FontClass
{
private:
	struct FontType
	{
		float left, right;
		int size;
	};

	struct  VertextType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;

	};

public:
	FontClass();
	FontClass(const FontClass&);
	~FontClass();

	bool Initialize(ID3D11Device*, char*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView** GetTexture();
	void BuildVertexArray(void*, char*, float, float);

private:
	bool LoadFontDate(char *);
	void ReleaseFontData();

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	FontType* m_Font;
	TextureClass* m_Texture;


};

#endif