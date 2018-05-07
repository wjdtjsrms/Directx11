////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "../Header_file/graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_LightShader = 0;
	m_LightFx = 0;
	m_Light = 0;
	m_Text = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	XMMATRIX BaseViewMatrix;



	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}


	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}


	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// ī�޶� ��ġ ���
	m_Camera->SetPosition(0.0f, 0.0f, -15.0f);
	m_Camera->Render();
	m_Camera->GetViewMatrix(BaseViewMatrix);
	
	m_Model = new ModelClass;
	if(!m_Model)
	{
		return false;
	}


	// ��� �и� �Ұ�
	result = m_Model->Initialize(m_D3D->GetDevice(), L"Resource/test.obj", L"Resource/model.txt", L"Resource/uv_snap.dds", L"Resource/uv_snap2.dds"); //change file
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_LightFx = new LightFxClass;
	if (!m_LightFx)
	{
		return false;
	}


	result = m_LightFx->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}


	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// light object �� ����
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(0.5f, 0.5f, 0.5f, 0.5f);
	m_Light->SetDirection(-1.0f, 0.0f, 0.0f);
	m_Light->setSpecularColor(1.0f, 0.0f, 0.0f, 1.0f);
	m_Light->SetSpecularPower(16.0f);

	m_Text = new TextClass;
	if (!m_Light){
		return false;
	}

	result = m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, BaseViewMatrix);
	if (!m_Light)
	{
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{

	if (m_Text){
		
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;

	}


	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}


	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	if (m_LightFx)
	{
		m_LightFx->Shutdown();
		delete m_LightFx;
		m_LightFx = 0;
	}


	if(m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}


	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame(int mouseX, int mouseY, int fps, int cpuPercent, float time)
{
	bool result;


	result = m_Text->SetMousePosition(mouseX, mouseY, m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	result = m_Text->setFps(fps, m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}

	result = m_Text->SetCpu(cpuPercent, m_D3D->GetDeviceContext());
	if (!result)
	{
		return false;
	}
	

	// �׷��� �� �׸���
	// ���� ���⼭ ���� ���� �����ϴ°� ������?
	result = Render(mouseX, mouseY);
	if(!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render(int mouseX, int mouseY)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix,orthoMatrix;
	XMVECTOR Direction, DiffuseColor, AmbeintColor, SpecularColor, CameraPostion;

	float SpecularPower;
	
	bool result;

	float rotationX;
	float rotationY;

	rotationX = (float)XM_PI * (mouseX*0.0008f);
	rotationY = (float)XM_PI * (mouseY*0.0008f);

	
	// ���۸� ���� ���� ����
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// View matrix �� �����ϰ� ī�޶��� �⺻ ���� ����
	m_Camera->Render();

	// ���̴� ���࿡ �ʿ��� ���� ���������� ����
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);
	CameraPostion = XMLoadFloat3(&m_Camera->GetPosition());

	

	// ���� ����� ������ ���������� ����
	m_Light->GetDirection(Direction);
	m_Light->GetDiffuseColor(DiffuseColor);
	m_Light->GetAmbientColor(AmbeintColor);
	m_Light->GetSpecularColor(SpecularColor);
	SpecularPower = m_Light->GetSpecularPower();


	// ��ü�� ȸ���� ����
	// ī�޶��� ȸ������ ��ü ����
	worldMatrix = worldMatrix*XMMatrixRotationY(-rotationX)*XMMatrixRotationX(-rotationY);

	//���� ���� ���� ���� device context �� ������
	m_Model->Render(m_D3D->GetDeviceContext());


	// �־��� ������ fx���� ������ �� ����
	HR(result = m_LightFx->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, Direction, DiffuseColor, AmbeintColor, CameraPostion, SpecularColor, SpecularPower, m_Model->GetTexture()));

	// ���ڵ� �Բ� ���ư��� �ʰ� �ϱ� ���� ���ο� worldmatrix ����
	// �и� �ʼ�
	m_D3D->GetWorldMatrix(worldMatrix);

	// 2d �������� ���� ���� �ѱ�
	m_D3D->TurnZBufferOff();
	m_D3D->TurnOnAlphaBlending();

	HR(m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix));

	// 2d �������� ���� ���� ����
	m_D3D->TurnOffAlphaBlending();
	m_D3D->TurnZBufferOn();



	// ���� �׸���
	m_D3D->EndScene();

	return true;
}

bool GraphicsClass::setValue() {

	


	return true;
}