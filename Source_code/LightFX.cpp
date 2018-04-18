#include "stdafx.h"
#include "../Header_file/LightFX.h"



LightFxClass::LightFxClass()
{
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;
	m_lightBuffer = 0;
	m_cameraBuffer = 0;
	m_lightBuffer = 0;

}

LightFxClass::LightFxClass(const LightFxClass&)
{
}

LightFxClass::~LightFxClass()
{
}

bool LightFxClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;


	result = InitializeShader(device, hwnd, L"Shader_code/Light.fx");
	if (!result)
	{
		return false;
	}
	return true;
}


void LightFxClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	LightFxClass();

	return;
}

bool LightFxClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX& worldMatrix, XMMATRIX& viewMatrix,
	XMMATRIX& projectionMatrix, XMVECTOR& lightDirection, XMVECTOR& diffuseColor, XMVECTOR& ambientColor, XMVECTOR& cameraPosition, XMVECTOR& specularColor, XMVECTOR& specularPower, ID3D11ShaderResourceView** textureArray)
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

bool LightFxClass::initializeDesc(ID3D11Device* device) {

	HRESULT result;

	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;


	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; //linear interpolation
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // 0.0f~1.0f if value is over that value is resize
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer);
	if (FAILED(result)) {
		return false;
	}


	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType); //buffer size is 16 * x ?
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}

bool LightFxClass::InitializeShader(ID3D11Device*  device, HWND hwnd, const WCHAR* fxFileName) {

	HRESULT result;
	ID3DBlob* errorMessage;
	ID3DBlob* fxShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	fxShaderBuffer = 0;

	//D3DX11CompileEffectFromFile fx 는 이거 밑에는 hlsl
	result = D3DCompileFromFile((LPCWSTR)fxFileName, NULL, NULL, NULL, "fx_5_0", 0, 0, &fxShaderBuffer, &errorMessage);
	
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

	
	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT; // x y or u v
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	D3DX11_PASS_DESC passdesc;
	mTech = m_fxShader->GetTechniqueByName("LightTech");
	mTech->GetPassByIndex(0)->GetDesc(&passdesc);
	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, passdesc.pIAInputSignature,
		passdesc.IAInputSignatureSize, &m_layout);


	if (FAILED(result))
	{
		return false;
	}



	fxShaderBuffer->Release();
	initializeDesc(device);


	return true;
}


void LightFxClass::ShutdownShader()
{
	// Release the light constant buffer.
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}
	if (m_cameraBuffer) {
		m_cameraBuffer->Release();
		m_cameraBuffer = 0;
	}

	// Release the sampler state.
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

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




bool LightFxClass::SetFXParameters(ID3D11DeviceContext* deviceContext, XMMATRIX& worldMatrix, XMMATRIX& viewMatrix,
	XMMATRIX& projectionMatrix, XMVECTOR& lightDirection, XMVECTOR& diffuseColor, XMVECTOR& ambientColor, XMVECTOR& cameraPosition, XMVECTOR& specularColor, XMVECTOR& specularPower, ID3D11ShaderResourceView** textureArray)
{



	XMStoreFloat4x4(&mWorldBuffer.world, worldMatrix);
	XMStoreFloat4x4(&mWorldBuffer.projection, projectionMatrix);
	XMStoreFloat4x4(&mWorldBuffer.view, viewMatrix);

	XMStoreFloat4(&mfxBuffer.ambientColor, ambientColor);
	XMStoreFloat4(&mfxBuffer.diffuseColor, diffuseColor);
	XMStoreFloat3(&mfxBuffer.lightDirection, lightDirection);
	XMStoreFloat4(&mfxBuffer.specularColor, specularColor);
	XMStoreFloat4(&mfxBuffer.specularPower, specularPower);


	XMStoreFloat4(&mCameraBuffer.cameraPosition, cameraPosition);



	mfxWorldViewProj = m_fxShader->GetVariableByName("gMatrix");
	mfxLight = m_fxShader->GetVariableByName("gLight");
	mfxCamera = m_fxShader->GetVariableByName("gCamera");




	
	return true;
}

bool LightFxClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	HRESULT result;

	result = mfxWorldViewProj->SetRawValue(&mWorldBuffer, 0, sizeof(mWorldBuffer));

	if (FAILED(result))
	{
		return false;
	}


	result = mfxLight->SetRawValue(&mfxBuffer, 0, sizeof(mfxBuffer));


	if (FAILED(result))
	{
		return false;
	}


	result = mfxCamera->SetRawValue(&mCameraBuffer, 0, sizeof(mCameraBuffer));


	if (FAILED(result))
	{
		return false;
	}




	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);




	// Set the vertex and pixel shaders that will be used to render this triangle.
	//deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	//deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	//deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	mTech->GetPassByIndex(indexCount)->Apply(0, deviceContext);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return true;
}