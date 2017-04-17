
// Includes...
#include "Ox93_TextureHandler.h"
#include "ROOT/Graphics/Ox93_D3D.h"

// Static Variables...
Ox93_TextureHandler* Ox93_TextureHandler::s_pxThis = nullptr;

Ox93_TextureHandler::Ox93_TextureHandler()
: m_xTextures()
{
}

bool Ox93_TextureHandler::Create()
{
	if (s_pxThis) { return false; }

	s_pxThis = new Ox93_TextureHandler;
	return s_pxThis->Init();
}

void Ox93_TextureHandler::Destroy()
{
	if (s_pxThis)
	{
		s_pxThis->Shutdown();
		s_pxThis = nullptr;
	}
}

bool Ox93_TextureHandler::Init()
{
	ID3D11Device* pxDevice = Ox93_D3D::GetDevice();
	HWND hWnd = Ox93_System::GetWindow();

	LoadAllTextures();

	return true;
}

void Ox93_TextureHandler::Shutdown()
{
	for (u_int u = 0; u < m_xTextures.size(); u++)
	{
		if (m_xTextures[u].m_pxTexture)
		{
			m_xTextures[u].m_pxTexture->Release();
		}
	}
	m_xTextures.clear();
}

ID3D11ShaderResourceView* Ox93_TextureHandler::GetTextureByHash(Ox93_Hash uTextureHash)
{
	Ox93_Assert(s_pxThis, "Attempting to GetTexture before creating the texture handler.");
	if (!s_pxThis) { return nullptr; }
	
	for (u_int u = 0; u < s_pxThis->m_xTextures.size(); u++)
	{
		if (s_pxThis->m_xTextures[u].m_uHash == uTextureHash)
		{
			return s_pxThis->m_xTextures[u].m_pxTexture;
		}
	}

	return nullptr;
}

void Ox93_TextureHandler::LoadAllTextures()
{
	WIN32_FIND_DATA xFindData;
	HANDLE hFile = FindFirstFile(L"Textures/*.dds", &xFindData);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		u_int uTextureNumber = 0;
		do {
			if (!(xFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				WCHAR* pwszFilename = new WCHAR[256];
				pwszFilename[0] = L'T';
				pwszFilename[1] = L'e';
				pwszFilename[2] = L'x';
				pwszFilename[3] = L't';
				pwszFilename[4] = L'u';
				pwszFilename[5] = L'r';
				pwszFilename[6] = L'e';
				pwszFilename[7] = L's';
				pwszFilename[8] = L'/';
				int i;
				for (i = 0; xFindData.cFileName[i]; i++)
				{
					if (i >= 247)
					{
						Ox93_Assert(false, "String copying failed, filename too long?");
						break;
					}

					pwszFilename[i + 9] = xFindData.cFileName[i];
				}
				pwszFilename[i + 9] = 0;

				Ox93_Texture_Handle* pxTexture = new Ox93_Texture_Handle;
				HRESULT hResult = DirectX::CreateDDSTextureFromFile(Ox93_D3D::GetDevice(), pwszFilename, nullptr, &(pxTexture->m_pxTexture));
				if (SUCCEEDED(hResult)) 
				{ 
					pxTexture->m_uHash = GetHash(pwszFilename);
					if (pxTexture->m_uHash != OX93_HASH_UNSET)
					{
						m_xTextures.push_back(*pxTexture);
					}
				}
				else
				{
					delete pxTexture;
					pxTexture = nullptr;
				}
			}
		} while (FindNextFile(hFile, &xFindData));
		FindClose(hFile);
	}
}