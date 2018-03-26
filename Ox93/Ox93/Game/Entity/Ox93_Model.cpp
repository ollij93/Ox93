#include "Ox93_Model.h"
#include "ROOT/Assert/Ox93_Assert.h"
#include "ROOT/Graphics/Ox93_D3D.h"
#include <fstream>

Ox93_Model::Ox93_Model()
: m_pxVertices(nullptr)
{
}

Ox93_Model::~Ox93_Model()
{
}

bool Ox93_Model::Init(Ox93_Hash uModelHash, Ox93_Hash uTextureHash)
{
	m_uTextureHash = uTextureHash;

	bool bFileLoaded = LoadModel(uModelHash);
	Ox93_Assert(bFileLoaded, "Failed to load a model file.");
	if (!bFileLoaded) { return false; }

	return InitBuffers();
}

void Ox93_Model::Shutdown()
{
	ReleaseModel();
	ShutdownBuffers();
}

bool Ox93_Model::InitBuffers()
{
	ID3D11Device* pxDevice = Ox93_D3D::GetDevice();
	Ox93_Assert(pxDevice, "Could not get the D3D device to initialize the models buffers too.");
	if (!pxDevice) { return false; }

	Ox93_Vertex* pxVertices = new Ox93_Vertex[m_uVertexCount];
	u_int* puIndices = new u_int[m_uIndexCount];
	if (!pxVertices || !puIndices) { return false; }


	for (u_int u = 0; u < m_uVertexCount; u++)
	{
		pxVertices[u] = m_pxVertices[u];
		puIndices[u] = u;
	}

	D3D11_BUFFER_DESC xVertexBufferDesc;
	xVertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	xVertexBufferDesc.ByteWidth = sizeof(Ox93_Vertex) * m_uVertexCount;
	xVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	xVertexBufferDesc.CPUAccessFlags = 0;
	xVertexBufferDesc.MiscFlags = 0;
	xVertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA xVertexData;
	xVertexData.pSysMem = pxVertices;
	xVertexData.SysMemPitch = 0;
	xVertexData.SysMemSlicePitch = 0;

	HRESULT hResult = pxDevice->CreateBuffer(&xVertexBufferDesc, &xVertexData, &m_pxVertexBuffer);
	if (FAILED(hResult)) { return false; }

	D3D11_BUFFER_DESC xIndexBufferDesc;
	xIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	xIndexBufferDesc.ByteWidth = sizeof(u_int) * m_uIndexCount;
	xIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	xIndexBufferDesc.CPUAccessFlags = 0;
	xIndexBufferDesc.MiscFlags = 0;
	xIndexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA xIndexData;
	xIndexData.pSysMem = puIndices;
	xIndexData.SysMemPitch = 0;
	xIndexData.SysMemSlicePitch = 0;

	hResult = pxDevice->CreateBuffer(&xIndexBufferDesc, &xIndexData, &m_pxIndexBuffer);
	if (FAILED(hResult)) { return false; }

	delete[] pxVertices;
	pxVertices = nullptr;

	delete[] puIndices;
	puIndices = nullptr;

	return true;
}

bool Ox93_Model::LoadModel(Ox93_Hash uModelHash)
{
	if (m_pxVertices) { return false; }
	if (uModelHash == OX93_HASH_UNSET) { return false; }

	// OJ - Moving to the hash system so find all files in the modesl folder until the hash matches
	WIN32_FIND_DATA xFindData;
	WCHAR* pwszFilename = new WCHAR[256];
	bool bFound = false;
	HANDLE hFile = FindFirstFile(L"Models/*.obj", &xFindData);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do {
			if (!(xFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				pwszFilename[0] = L'M';
				pwszFilename[1] = L'o';
				pwszFilename[2] = L'd';
				pwszFilename[3] = L'e';
				pwszFilename[4] = L'l';
				pwszFilename[5] = L's';
				pwszFilename[6] = L'/';
				int i;
				for (i = 0; xFindData.cFileName[i]; i++)
				{
					if (i >= 249)
					{
						Ox93_Assert(false, "String copying failed, filename too long?");
						break;
					}

					pwszFilename[i + 7] = xFindData.cFileName[i];
				}
				pwszFilename[i + 7] = 0;

				if (GetHash(pwszFilename) == uModelHash)
				{
					bFound = true;
					break;
				}
			}
		} while (FindNextFile(hFile, &xFindData));
		FindClose(hFile);
	}

	if (!bFound) { return false; }

	// Open the file
	std::ifstream xIFStream;
	xIFStream.open(pwszFilename);
	if (xIFStream.fail()) { return false; }

	// Loop through the file to count the numbers of different vertices
	int iVertexCount = 0;
	int iTextCoordCount = 0;
	int iNormalCount = 0;
	int iFaceCount = 0;

	char cInput;
	xIFStream.get(cInput);
	while (!xIFStream.eof())
	{
		// Check for the starting code
		switch (cInput)
		{
			case 'v':
			{
				xIFStream.get(cInput);
				if (cInput == ' ')
				{
					// This line contains information on a Vertex
					iVertexCount++;
				}
				if (cInput == 't')
				{
					// This line contains information on a texture
					iTextCoordCount++;
				}
				if (cInput == 'n')
				{
					// This line contains information on a normal
					iNormalCount++;
				}
				break;
			}
			case 'f':
			{
				xIFStream.get(cInput);
				if (cInput == ' ')
				{
					// This line contains information about a face
					iFaceCount++;

					// Count how many spaces are in the line (there will be Nspaces+1 points in the face, so Nspaces-1 triangles)
					int iSpaces = 0;
					xIFStream.get(cInput);
					while (cInput != '\n')
					{
						if (cInput == ' ')
						{
							iSpaces++;
						}
						xIFStream.get(cInput);
					}

					iFaceCount += (iSpaces - 2);
				}
				break;
			}
			default:
				break;
		}

		// Loop through the rest of the line
		while(cInput != '\n')
		{
			xIFStream.get(cInput);
		}
		xIFStream.get(cInput);
	}

	// Reset the file to the begining by closing and reopening it
	xIFStream.close();
	xIFStream.open(pwszFilename);
	if (xIFStream.fail()) { return false; }

	// Create the buffers for the vertices, texture coords and normals 
	Ox93_Vector_3* pxVertices = new Ox93_Vector_3[iVertexCount];
	Ox93_Vector_2* pxTextCoords = new Ox93_Vector_2[iTextCoordCount];
	Ox93_Vector_3* pxNormals = new Ox93_Vector_3[iNormalCount];

	struct FaceType
	{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
	};

	FaceType* pxFaces = new FaceType[iFaceCount];

	int iVertex = 0;
	int iCoord = 0;
	int iNormal = 0;
	int iFace = 0;
	xIFStream.get(cInput);
	while (!xIFStream.eof())
	{
		switch (cInput)
		{
			case 'v':
			{
				xIFStream.get(cInput);
				if (cInput == ' ')
				{
					// Read in the vertex information
					xIFStream >> pxVertices[iVertex].x >> pxVertices[iVertex].y >> pxVertices[iVertex].z;
					pxVertices[iVertex].z = -pxVertices[iVertex].z;
					iVertex++;
				}
				if (cInput == 't')
				{
					// Read in the texture information
					xIFStream >> pxTextCoords[iCoord].x >> pxTextCoords[iCoord].y;
					pxTextCoords[iCoord].y = 1.f - pxTextCoords[iCoord].y;
					iCoord++;
				}
				if (cInput == 'n')
				{
					// Read in the normal information
					xIFStream >> pxNormals[iNormal].x >> pxNormals[iNormal].y >> pxNormals[iNormal].z;
					pxNormals[iNormal].z = -pxNormals[iNormal].z;
					iNormal++;
				}
				break;
			}
			case 'f':
			{
				// Read in the face information
				while (cInput != '\n')
				{
					char cInput2;
					// Read the face data in backwards to convert it to a left hand system from right hand system.
					xIFStream >> pxFaces[iFace].vIndex1 >> cInput2 >> pxFaces[iFace].tIndex1 >> cInput2 >> pxFaces[iFace].nIndex1
							>> pxFaces[iFace].vIndex3 >> cInput2 >> pxFaces[iFace].tIndex3 >> cInput2 >> pxFaces[iFace].nIndex3
							>> pxFaces[iFace].vIndex2 >> cInput2 >> pxFaces[iFace].tIndex2 >> cInput2 >> pxFaces[iFace].nIndex2;

					iFace++;

					// Read in the quads or higher order polys if necessary
					xIFStream.get(cInput);
					while (cInput != '\n')
					{
						pxFaces[iFace].vIndex1 = pxFaces[iFace - 1].vIndex1;
						pxFaces[iFace].tIndex1 = pxFaces[iFace - 1].tIndex1;
						pxFaces[iFace].nIndex1 = pxFaces[iFace - 1].nIndex1;

						xIFStream >> pxFaces[iFace].vIndex2 >> cInput2 >> pxFaces[iFace].tIndex2 >> cInput2 >> pxFaces[iFace].nIndex2;

						pxFaces[iFace].vIndex3 = pxFaces[iFace - 1].vIndex2;
						pxFaces[iFace].tIndex3 = pxFaces[iFace - 1].tIndex2;
						pxFaces[iFace].nIndex3 = pxFaces[iFace - 1].nIndex2;

						iFace++;

						xIFStream.get(cInput);
					}
				}
				break;
			}
			default:
				break;
		}

		while (cInput != '\n')
		{
			xIFStream.get(cInput);
		}
		xIFStream.get(cInput);
	}

	// Loop through the faces and get the relevant data into the final vertices buffer
	m_uVertexCount = 3 * iFaceCount;
	m_uIndexCount = m_uVertexCount;
	m_pxVertices = new Ox93_Vertex[m_uVertexCount];

	for (int i = 0; i < iFaceCount; i++)
	{
		m_pxVertices[3*i].m_xPosition = pxVertices[pxFaces[i].vIndex1 - 1];
		m_pxVertices[3*i].m_xNormal = pxNormals[pxFaces[i].nIndex1 - 1];
		m_pxVertices[3*i].m_xTexture = pxTextCoords[pxFaces[i].tIndex1 - 1];

		m_pxVertices[3*i + 1].m_xPosition = pxVertices[pxFaces[i].vIndex2 - 1];
		m_pxVertices[3*i + 1].m_xNormal = pxNormals[pxFaces[i].nIndex2 - 1];
		m_pxVertices[3*i + 1].m_xTexture = pxTextCoords[pxFaces[i].tIndex2 - 1];

		m_pxVertices[3*i + 2].m_xPosition = pxVertices[pxFaces[i].vIndex3 - 1];
		m_pxVertices[3*i + 2].m_xNormal = pxNormals[pxFaces[i].nIndex3 - 1];
		m_pxVertices[3*i + 2].m_xTexture = pxTextCoords[pxFaces[i].tIndex3 - 1];
	}

	delete[] pxVertices;
	pxVertices = nullptr;

	delete[] pxTextCoords;
	pxTextCoords = nullptr;

	delete[] pxNormals;
	pxNormals = nullptr;

	delete[] pxFaces;
	pxFaces = nullptr;

	return true;
}

void Ox93_Model::ReleaseModel()
{
	if (m_pxVertices)
	{
		delete[] m_pxVertices;
		m_pxVertices = nullptr;
	}
	return;
}

void Ox93_Model::Render()
{
	ID3D11DeviceContext* pxDeviceContext = Ox93_D3D::GetDeviceContext();
	if (!pxDeviceContext) { return; }

	u_int uSize = sizeof(Ox93_Vertex);
	u_int uOffset = 0;

	pxDeviceContext->IASetVertexBuffers(0, 1, &m_pxVertexBuffer, &uSize, &uOffset);
	pxDeviceContext->IASetIndexBuffer(m_pxIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pxDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Ox93_Model::ShutdownBuffers()
{
	if (m_pxIndexBuffer)
	{
		m_pxIndexBuffer->Release();
		m_pxIndexBuffer = nullptr;
	}

	if (m_pxVertexBuffer)
	{
		m_pxVertexBuffer->Release();
		m_pxVertexBuffer = nullptr;
	}
}