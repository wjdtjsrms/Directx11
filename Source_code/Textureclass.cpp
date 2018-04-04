#include "../Header_file/Textureclass.h"

TextureClass::TextureClass()
{
	m_texture[0] = NULL;
	m_texture[1] =	NULL;
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2){

	HRESULT result;
	result = D3DX11CreateShaderResourceViewFromFile(device, filename1, NULL, NULL, &m_texture[0], NULL);

	if (FAILED(result))
	{
		return false;
	}

	result = D3DX11CreateShaderResourceViewFromFile(device, filename2, NULL, NULL, &m_texture[1], NULL);

	if (FAILED(result))
	{
		return false;
	}


	return true;

}

bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename){

	HRESULT result;
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_texture[0], NULL);

	if (FAILED(result))
	{
		return false;
	}


	return true;
}

void TextureClass::Shutdown()
{
	// Release the texture resource.
	if (m_texture[0])
	{
		m_texture[0]->Release();
		m_texture[0] = 0;
	}

	if (m_texture[1])
	{
		m_texture[1]->Release();
		m_texture[1] = 0;
	}


	return;
}

ID3D11ShaderResourceView** TextureClass::GetTexture()
{
	return m_texture;
}

