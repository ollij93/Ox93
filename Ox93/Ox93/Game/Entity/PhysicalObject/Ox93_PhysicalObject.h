#ifndef __OX93_PHYSICALOBJECT_H__
#define __OX93_PHYSICALOBJECT_H__

// Includes...
#include "Game/Entity/Ox93_Entity_Renderable.h"
#include "Game/Entity/Ox93_Model.h"
#include "ROOT/Physics/Ox93_AABB.h"

// Forward Declarations...
class Ox93_Specification;

class Ox93_PhysicalObject : public Ox93_Entity_Renderable
{
public:
	Ox93_PhysicalObject(u_int uClassification);
	~Ox93_PhysicalObject();

	static Ox93_Entity* Create() { return new Ox93_PhysicalObject(OX93_CLASS_PHYSICALOBJECT); }

	virtual void InitFromSpecification(const Ox93_Specification* pxSpecification) override;

	virtual void Update() override;
	virtual void Render() override { if(m_pxModel) { m_pxModel->Render(); } }

	virtual ID3D11ShaderResourceView* GetTexture() const override { return m_pxModel ? m_pxModel->GetTexture() : nullptr; }
	virtual u_int GetIndexCount() const override { return m_pxModel ? m_pxModel->GetIndexCount() : 0; }

private:
	Ox93_Model* m_pxModel;
	// TODO-OJ This should be a generic collision object not specifically an AABB
	Ox93_AABB m_xAABB;

	typedef Ox93_Entity_Renderable PARENT;
};

#endif // ifndef __OX93_PHYSICALOBJECT_H__