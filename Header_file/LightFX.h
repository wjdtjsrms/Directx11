#ifndef _LIGHTFXCLASS_H_
#define _LIGHTFXCLASS_H_

#include "pch.h"



using namespace DirectX;
using namespace std;

class LightFxClass
{

private:

	struct MatrixBufferType
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		XMFLOAT4 specularColor;
		XMFLOAT4 specularPower;
	};
	struct  CameraBufferType
	{
		XMFLOAT4 cameraPosition;

	};


public:
	LightFxClass();
	LightFxClass(const LightFxClass&);
	~LightFxClass();


	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX&, XMMATRIX&, XMMATRIX&, XMVECTOR&, XMVECTOR&, XMVECTOR&, XMVECTOR&, XMVECTOR&, XMVECTOR&, ID3D11ShaderResourceView**);

private:
	bool initializeDesc(ID3D11Device*);
	bool InitializeShader(ID3D11Device*, HWND, const WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, const WCHAR*);

	bool SetFXParameters(ID3D11DeviceContext*, XMMATRIX&, XMMATRIX&, XMMATRIX&, XMVECTOR&, XMVECTOR&, XMVECTOR&, XMVECTOR&, XMVECTOR&, XMVECTOR&, ID3D11ShaderResourceView**);
	bool RenderShader(ID3D11DeviceContext*, int);

private:

	ID3DX11Effect* m_fxShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;


	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_cameraBuffer;


	ID3DX11EffectTechnique* mTech;

	ID3DX11EffectVariable* mfxWorldViewProj;
	ID3DX11EffectVariable* mfxLight;
	ID3DX11EffectVariable* mfxCamera;

	MatrixBufferType mWorldBuffer;
	LightBufferType mfxBuffer;
	CameraBufferType mCameraBuffer;




};


#endif  _LIGHTFXCLASS_H_