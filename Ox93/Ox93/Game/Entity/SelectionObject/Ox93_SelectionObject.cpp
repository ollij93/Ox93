// Includes...
#include "Ox93_SelectionObject.h"
#include "Game/Entity/PhysicalObject/Ox93_PhysicalObject.h"
#include "ROOT/Specification/Ox93_Specification.h"
#include "ROOT/Specification/Ox93_Specification_Hash_IDs.h"
#include "ROOT/Specification/Ox93_Specification_System.h"

Ox93_SelectionObject::Ox93_SelectionObject(u_int uClassification)
: PARENT(uClassification)
, m_pxModel(nullptr)
, m_bHidden(false)
{
	Ox93_LightShader::AddToRenderList(this);
}

Ox93_SelectionObject::~Ox93_SelectionObject()
{
	Ox93_LightShader::RemoveFromRenderList(this);

	if (m_pxModel)
	{
		m_pxModel->Shutdown();
		delete m_pxModel;
		m_pxModel = nullptr;
	}
}

void Ox93_SelectionObject::InitFromSpecification(const Ox93_Specification* pxSpecification)
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

Ox93_PhysicalObject* Ox93_SelectionObject::CreatePhysicalObject() const
{
	Ox93_PhysicalObject* pxPhysObject = static_cast<Ox93_PhysicalObject*>( Ox93_PhysicalObject::Create() );

	const Ox93_Specification* pxSpecification = Ox93_Specification_System::GetSpecificationByHash(GetSpecificationHash());
	pxPhysObject->InitFromSpecification(pxSpecification);

	pxPhysObject->SetPosition(GetPosition());
	pxPhysObject->SetOrientation(GetOrientation());

	return pxPhysObject;
}