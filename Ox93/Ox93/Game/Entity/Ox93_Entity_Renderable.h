#ifndef __OX93_ENTITY_RENDERABLE_H__
#define __OX93_ENTITY_RENDERABLE_H__

// Includes...
#include <d3d11.h>
#include "Game/Entity/Ox93_Entity.h"

class Ox93_Entity_Renderable : public Ox93_Entity
{
public:
	Ox93_Entity_Renderable(u_int uClassification);
	~Ox93_Entity_Renderable();

	virtual void Render() = 0;

	virtual ID3D11ShaderResourceView* GetTexture() const { return nullptr; }
	virtual u_int GetIndexCount() const = 0;

private:
	typedef Ox93_Entity PARENT;
};

#endif // ifndef __OX93_ENTITY_RENDERABLE_H__
