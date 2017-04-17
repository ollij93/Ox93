#ifndef __OX93_TEXTUREHANDLER_H__
#define __OX93_TEXTUREHANDLER_H__

// Includes...
#include "Ox93_Core.h"
#include "ROOT/Graphics/Shader/Ox93_LightShader.h"

struct Ox93_Texture_Handle
{
	Ox93_Hash m_uHash;
	ID3D11ShaderResourceView* m_pxTexture;
};

class Ox93_TextureHandler
{
public:
	static bool Create();
	static void Destroy();

	static ID3D11ShaderResourceView* GetTextureByHash( Ox93_Hash uTextureHash );

protected:
	Ox93_TextureHandler();
	~Ox93_TextureHandler() {}

	bool Init();
	void Shutdown();

	void LoadAllTextures();

private:
	std::vector<Ox93_Texture_Handle> m_xTextures;

	static Ox93_TextureHandler* s_pxThis;
};
#endif // ifndef __OX93_TEXTUREHANDLER_H__
