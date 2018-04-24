#ifndef _LIGHTFXCLASS_H_
#define _LIGHTFXCLASS_H_

#include "pch.h"



using namespace DirectX;
using namespace std;




class LightFxClass
{

private:

	struct Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT4 Color;
	};



public:
	LightFxClass();
	LightFxClass(const LightFxClass&);
	~LightFxClass();


	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX&, XMMATRIX&, XMMATRIX&, XMVECTOR&, XMVECTOR&, XMVECTOR&, XMVECTOR&, XMVECTOR&, float, ID3D11ShaderResourceView**);

private:
	bool initializeDesc(ID3D11Device*);
	bool InitializeShader(ID3D11Device*, HWND, const WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);
	bool SetFXParameters(ID3D11DeviceContext*, XMMATRIX&, XMMATRIX&, XMMATRIX&, XMVECTOR&, XMVECTOR&, XMVECTOR&, XMVECTOR&, XMVECTOR&, float, ID3D11ShaderResourceView**);
	void RenderShader(ID3D11DeviceContext*, int);

	bool BuildGeometryBuffers(ID3D11Device*);
private:

	ID3DX11Effect* m_fxShader;
	ID3D11InputLayout* m_layout;

	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;

	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;
	XMFLOAT4 Color{ 0.0f, 0.0f, 0.0f, 0.0f };

};


#endif  _LIGHTFXCLASS_H_