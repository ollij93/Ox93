#ifndef __OX93_SKYBOX_H__
#define __OX93_SKYBOX_H__

// Includes...
#include "Game/Entity/Ox93_Entity_Renderable.h"
#include "ROOT/Ox93_TextureHandler.h"
#include "ROOT/Graphics/Ox93_D3D.h"

class Ox93_Skybox
{
public:
	Ox93_Skybox();
	~Ox93_Skybox();

	static Ox93_Skybox* Create() { return new Ox93_Skybox; }

	void InitFromSpecification(const Ox93_Specification* pxSpecification);

	void Render();

	u_int GetIndexCount() const { return 6 * m_uVerticalFaces * m_uHorizontalFaces; }
	ID3D11ShaderResourceView* GetTexture() const { return Ox93_TextureHandler::GetTextureByHash(m_uTextureHash); }

	void SetPosition(Ox93_Vector_3 xPosition) { m_xPosition = xPosition; }
	Ox93_Vector_3 GetPosition() const { return m_xPosition; }

	static Ox93_Skybox* GetActiveSkybox() { return s_pxActiveSkybox; }

protected:
	virtual bool InitBuffers(ID3D11Device* pxDevice);

private:
	float m_fRadius;
	u_int m_uVerticalFaces;
	u_int m_uHorizontalFaces;
	Ox93_Hash m_uTextureHash;
	Ox93_Vector_3 m_xPosition;

	ID3D11Buffer* m_pxVertexBuffer;
	ID3D11Buffer* m_pxIndexBuffer;

	static Ox93_Skybox* s_pxActiveSkybox;

	typedef Ox93_Entity_Renderable PARENT;
};

#endif __OX93_SKYBOX_H__

