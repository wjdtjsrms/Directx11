#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_

#include <D3D11.h>
#include <xnamath.h>

class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void setSpecularColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetAmbientColor(float, float, float, float);
	void SetSpecularPower(float);

	void GetDiffuseColor(XMVECTOR&);
	void GetDirection(XMVECTOR&);
	void GetAmbientColor(XMVECTOR&);
	void GetSpecularColor(XMVECTOR&);
	void GetSpecularPower(float&);

private:
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_direction;
	XMFLOAT4 m_specularColor;
	float m_specularPower;
	
};



#endif