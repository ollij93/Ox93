#include "Ox93_PhysicalObject.h"
#include "Game/Entity/Ox93_Model.h"
#include "ROOT/Graphics/Shader/Ox93_DepthShader.h"
#include "ROOT/Specification/Ox93_Specification.h"
#include "ROOT/Specification/Ox93_Specification_Hash_IDs.h"


Ox93_PhysicalObject::Ox93_PhysicalObject(u_int uClassification)
: PARENT(uClassification)
, m_pxModel(nullptr)
, m_xAABB(1.f, 1.f, 1.f)
{
	Ox93_LightShader::AddToRenderList(this);
	Ox93_DepthShader::AddToRenderList(this);
}

Ox93_PhysicalObject::~Ox93_PhysicalObject()
{
	Ox93_LightShader::RemoveFromRenderList(this);
	Ox93_DepthShader::RemoveFromRenderList(this);

	if (m_pxModel)
	{
		m_pxModel->Shutdown();
		delete m_pxModel;
		m_pxModel = nullptr;
	}
}

void Ox93_PhysicalObject::Update()
{
	PARENT::Update();

	m_xAABB.SetPosition(m_xPosition);
}

void Ox93_PhysicalObject::InitFromSpecification(const Ox93_Specification* pxSpecification)
{
	if (pxSpecification)
	{
		PARENT::InitFromSpecification(pxSpecification);

		const Ox93_Hash uModelHash = pxSpecification->GetValueAsHash(uOX93_SPEC_PARAM_MODEL, OX93_HASH_UNSET);
		const Ox93_Hash uTextureHash = pxSpecification->GetValueAsHash(uOX93_SPEC_PARAM_TEXTURE, OX93_HASH_UNSET);

		if (m_pxModel)
		{
			m_pxModel->Shutdown();
			delete m_pxModel;
			m_pxModel = nullptr;
		}

		m_pxModel = new Ox93_Model;
		m_pxModel->Init(uModelHash, uTextureHash);
	}
}

