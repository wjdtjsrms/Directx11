#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_

#include "../Header_file/Fontclass.h"
#include "../Header_file/Fontshaderclass.h"


class TextClass
{


private: 
	struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};
	struct  VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;

	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, XMMATRIX&);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX&, XMMATRIX&);
	bool SetMousePosition(int,int,ID3D11DeviceContext*);
	bool setFps(int, ID3D11DeviceContext*);
	bool SetCpu(int, ID3D11DeviceContext*);
	bool SetRenderCount(int, ID3D11DeviceContext*);

private:
	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseSentence(SentenceType**);
	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, XMMATRIX&, XMMATRIX&);

private:
	FontClass* m_Font;
	Fontshaderclass* m_FontShader;
	int m_screenWidth, m_screenHeight;
	XMFLOAT4X4 m_baseViewMatrix;

	SentenceType* m_sentence1;
	SentenceType* m_sentence2;
	SentenceType* m_sentence3;
	SentenceType* m_sentence4;

};


#endif