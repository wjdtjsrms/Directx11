#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

#include <D3D11.h>
#include <D3DX11tex.h>

class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device*, WCHAR*, WCHAR*); //wchar = char ? load by name
	bool Initialize(ID3D11Device*, WCHAR*); //wchar = char ? load by name
	void Shutdown();
	ID3D11ShaderResourceView** GetTexture();

private:
	//연결 리스트로 구현해 보기
	ID3D11ShaderResourceView* m_texture[2];

};

#endif