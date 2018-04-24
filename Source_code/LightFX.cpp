#include "stdafx.h"
#include "../Header_file/LightFX.h"



LightFxClass::LightFxClass()
{

	m_fxShader = 0;
	m_layout = 0;
	mTech = 0;
	mfxWorldViewProj = 0;
	mBoxVB = 0;
	mBoxIB = 0;


}

LightFxClass::LightFxClass(const LightFxClass&)
{
}

LightFxClass::~LightFxClass()
{
}

void LightFxClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

void LightFxClass::ShutdownShader()
{
	// Release the light constant buffer.


	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if (m_fxShader) {
		m_fxShader->Release();
		m_fxShader = 0;
	}


	if (mBoxIB) {
		mBoxIB->Release();
		mBoxIB = 0;
	}
	if (mBoxVB) {
		mBoxVB->Release();
		mBoxVB = 0;
	}

	return;
}

void LightFxClass::OutputShaderErrorMessage(ID3DBlob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}




bool LightFxClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;


	result = InitializeShader(device, hwnd, L"Shader_code/color.fx");
	if (!result)
	{
		return false;
	}
	return true;
}


bool LightFxClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX& worldMatrix, XMMATRIX& viewMatrix,
	XMMATRIX& projectionMatrix, XMVECTOR& lightDirection, XMVECTOR& diffuseColor, XMVECTOR& ambientColor, XMVECTOR& cameraPosition, XMVECTOR& specularColor, float specularPower, ID3D11ShaderResourceView** textureArray)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetFXParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, lightDirection, diffuseColor, ambientColor, cameraPosition, specularColor, specularPower, textureArray);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}



bool LightFxClass::InitializeShader(ID3D11Device*  device, HWND hwnd, const WCHAR* fxFileName) {

	HRESULT result;

	ID3DBlob* errorMessage=0;
	ID3DBlob* fxShaderBuffer=0;

	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif


	result = D3DCompileFromFile((LPCWSTR)fxFileName, NULL, NULL, NULL, "fx_5_0",
		shaderFlags, 0, &fxShaderBuffer, &errorMessage);

	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, fxFileName);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, fxFileName, L"Missing fx File", MB_OK);
		}

		return false;
	}

	result = D3DX11CreateEffectFromMemory(fxShaderBuffer->GetBufferPointer(), fxShaderBuffer->GetBufferSize(),
		0, device, &m_fxShader);


	if (FAILED(result))
	{
		return false;
	}

	fxShaderBuffer->Release();

	mTech = m_fxShader->GetTechniqueByName("ColorTech");
	mfxWorldViewProj = m_fxShader->GetVariableByName("gWorldViewProj")->AsMatrix();

	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	
	// Create the vertex input layout.
	result = device->CreateInputLayout(vertexDesc, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_layout);


	if (FAILED(result))
	{
		return false;
	}


	return true;
}







bool LightFxClass::SetFXParameters(ID3D11DeviceContext* deviceContext, XMMATRIX& worldMatrix, XMMATRIX& viewMatrix,
	XMMATRIX& projectionMatrix, XMVECTOR& lightDirection, XMVECTOR& diffuseColor, XMVECTOR& ambientColor, XMVECTOR& cameraPosition, XMVECTOR& specularColor, float specularPower, ID3D11ShaderResourceView** textureArray)
{
	HRESULT result;


	XMMATRIX worldViewProj = worldMatrix * viewMatrix*projectionMatrix;

	
	result=mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	if (FAILED(result))
	{
		return false;
	}

	
	return true;
}

void LightFxClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{



	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	D3DX11_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; p++) {

		mTech->GetPassByIndex(p)->Apply(0, deviceContext);

		deviceContext->DrawIndexed(36, 0, 0);
	}

	

	return;
}


bool LightFxClass::BuildGeometryBuffers(ID3D11Device* device)
{
	HRESULT result;

	// Create vertex buffer
	Vertex vertices[] =
	{ 
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f } },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4{ 0.0f, 0.0f, 0.0f, 1.0f } },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f } },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f } },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f } },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4{ 1.0f, 1.0f, 0.0f, 1.0f } },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4{ 0.0f, 1.0f, 1.0f, 1.0f } },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4{ 1.0f, 0.0f, 1.0f, 1.0f } }
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	result=device->CreateBuffer(&vbd, &vinitData, &mBoxVB);

	if (FAILED(result)) {
		return false;
	}


	// Create the index buffer

	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	result=device->CreateBuffer(&ibd, &iinitData, &mBoxIB);

	if (FAILED(result)) {
		return false;
	}

	return true;
}
