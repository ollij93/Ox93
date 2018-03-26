#pragma once

#ifndef OX93_SELECTIONOBJECT_H__
#define OX93_SELECTIONOBJECT_H__

// Includes...
#include "Game/Entity/Ox93_Entity_Renderable.h"
#include "Game/Entity/Ox93_Model.h"

// Forward Declarations...
class Ox93_Specification;
class Ox93_PhysicalObject;

class Ox93_SelectionObject : public Ox93_Entity_Renderable
{
public:
	Ox93_SelectionObject(u_int uClassification);
	~Ox93_SelectionObject();

	static Ox93_Entity* Create() { return new Ox93_SelectionObject(OX93_CLASS_SELECTIONOBJECT); }
	Ox93_PhysicalObject* CreatePhysicalObject() const;

	virtual void ReadFromChunkStream(const Ox93_ChunkStream& xChunkStream) override {}
	virtual void WriteToChunkStream(Ox93_ChunkStream& xChunkStream) const override {}

	virtual void InitFromSpecification(const Ox93_Specification* pxSpecification) override;

	virtual void Render() override { if (m_pxModel) { m_pxModel->Render(); } }

	virtual ID3D11ShaderResourceView* GetTexture() const override { return m_pxModel ? m_pxModel->GetTexture() : nullptr; }
	virtual u_int GetIndexCount() const override { return m_pxModel ? m_pxModel->GetIndexCount() : 0; }

	void Hide() { if(!m_bHidden) { Ox93_LightShader::RemoveFromRenderList(this); } }
	void Unhide() { if (m_bHidden) { Ox93_LightShader::AddToRenderList(this); } }
	bool IsVisible() const { return !m_bHidden; }

private:
	Ox93_Model* m_pxModel;
	bool m_bHidden;

	typedef Ox93_Entity_Renderable PARENT;
};

#endif // ifndef OX93_SELECTIONOBJECT_H__