// Includes...
#include "Ox93_Terrain.h"
#include "Game/Entity/PhysicalObject/Ox93_PhysicalObject.h"
#include "Game/LoadSave/ChunkStream/Ox93_ChunkStream.h"
#include "Game/World/Terrain/Ox93_Biome.h"
#include "ROOT/Graphics/Ox93_D3D.h"
#include "ROOT/Graphics/Shader/Ox93_TerrainShader.h"

#define OX93_TERRAIN_PERLIN_VALUE 0.f

Ox93_Terrain::Ox93_Terrain()
: m_afHeightMap()
, m_apxPhysicalObjects()
, m_xPosition(Ox93_Math::ZeroVector3)
, m_pxVertexBuffer(nullptr)
, m_pxIndexBuffer(nullptr)
{
	memset(m_afHeightMap, 0, sizeof(m_afHeightMap));
	memset(m_apxPhysicalObjects, NULL, sizeof(m_apxPhysicalObjects));

	Ox93_TerrainShader::AddToRenderList(this);
}

Ox93_Terrain::~Ox93_Terrain()
{
	Ox93_TerrainShader::RemoveFromRenderList(this);
}

void Ox93_Terrain::Render()
{
	ID3D11DeviceContext* pxDeviceContext = Ox93_D3D::GetDeviceContext();
	if (!pxDeviceContext) { return; }

	u_int uSize = sizeof(Ox93_TerrainVertex);
	u_int uOffset = 0;

	pxDeviceContext->IASetVertexBuffers(0, 1, &m_pxVertexBuffer, &uSize, &uOffset);
	pxDeviceContext->IASetIndexBuffer(m_pxIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pxDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Ox93_Terrain::InitBuffers(ID3D11Device* pxDevice, int iTileX, int iTileZ, noise::module::Perlin xModule)
{
	u_int* puIndices = new u_int[6 * OX93_TERRAIN_NUMSIDEFACES * OX93_TERRAIN_NUMSIDEFACES];
	for (u_int u = 0; u < 6 * OX93_TERRAIN_NUMSIDEFACES * OX93_TERRAIN_NUMSIDEFACES; u++) {
		puIndices[u] = u;
	}

	Ox93_TerrainVertex* pxVertices = new Ox93_TerrainVertex[6 * OX93_TERRAIN_NUMSIDEFACES * OX93_TERRAIN_NUMSIDEFACES];
	for (u_int uX = 0; uX < OX93_TERRAIN_NUMSIDEFACES; uX++) {
		for (u_int uZ = 0; uZ < OX93_TERRAIN_NUMSIDEFACES; uZ++) {
			// POSITIONS
			const float fX = static_cast<float>(uX) * fOX93_TERRAIN_SCALE;
			const float fZ = static_cast<float>(uZ) * fOX93_TERRAIN_SCALE;

			const Ox93_Vector_3 xCorner0(fX,                       m_afHeightMap[uX][uZ],         fZ);                       // 0, 0
			const Ox93_Vector_3 xCorner1(fX + fOX93_TERRAIN_SCALE, m_afHeightMap[uX + 1][uZ],     fZ);                       // 1, 0
			const Ox93_Vector_3 xCorner2(fX,                       m_afHeightMap[uX][uZ + 1],     fZ + fOX93_TERRAIN_SCALE); // 0, 1
			const Ox93_Vector_3 xCorner3(fX + fOX93_TERRAIN_SCALE, m_afHeightMap[uX + 1][uZ + 1], fZ + fOX93_TERRAIN_SCALE); // 1, 1

			const Ox93_Vector_3 xPos0 = xCorner0;
			const Ox93_Vector_3 xPos1 = xCorner2;
			const Ox93_Vector_3 xPos2 = xCorner1;

			const Ox93_Vector_3 xPos3 = xCorner3;
			const Ox93_Vector_3 xPos4 = xCorner1;
			const Ox93_Vector_3 xPos5 = xCorner2;

			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 0].m_xPosition = xPos0;
			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 1].m_xPosition = xPos1;
			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 2].m_xPosition = xPos2;

			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 3].m_xPosition = xPos3;
			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 4].m_xPosition = xPos4;
			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 5].m_xPosition = xPos5;

			// NORMALS
			const Ox93_Vector_3 xNormal1 = (xPos1 ^ xPos2) - (xPos0 ^ xPos2) + (xPos0 ^ xPos1);
			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 0].m_xNormal = xNormal1;
			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 1].m_xNormal = xNormal1;
			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 2].m_xNormal = xNormal1;

			const Ox93_Vector_3 xNormal2 = (xPos4 ^ xPos5) - (xPos3 ^ xPos5) + (xPos3 ^ xPos4);
			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 3].m_xNormal = xNormal2;
			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 4].m_xNormal = xNormal2;
			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 5].m_xNormal = xNormal2;

			// COLORS
			const Ox93_Color xColor0 = Ox93_Biome::GetTerrainColorAtPosition(uX     + iTileX * OX93_TERRAIN_NUMSIDEFACES, uZ     + iTileZ * OX93_TERRAIN_NUMSIDEFACES, xModule);
			const Ox93_Color xColor1 = Ox93_Biome::GetTerrainColorAtPosition(uX + 1 + iTileX * OX93_TERRAIN_NUMSIDEFACES, uZ     + iTileZ * OX93_TERRAIN_NUMSIDEFACES, xModule);
			const Ox93_Color xColor2 = Ox93_Biome::GetTerrainColorAtPosition(uX     + iTileX * OX93_TERRAIN_NUMSIDEFACES, uZ + 1 + iTileZ * OX93_TERRAIN_NUMSIDEFACES, xModule);
			const Ox93_Color xColor3 = Ox93_Biome::GetTerrainColorAtPosition(uX + 1 + iTileX * OX93_TERRAIN_NUMSIDEFACES, uZ + 1 + iTileZ * OX93_TERRAIN_NUMSIDEFACES, xModule);

			// Apply the colors
			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 0].m_xColor = xColor0;
			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 1].m_xColor = xColor2;
			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 2].m_xColor = xColor1;

			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 3].m_xColor = xColor3;
			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 4].m_xColor = xColor1;
			pxVertices[(uX * OX93_TERRAIN_NUMSIDEFACES + uZ) * 6 + 5].m_xColor = xColor2;
		}
	}

	D3D11_BUFFER_DESC xVertexBufferDesc;
	xVertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	xVertexBufferDesc.ByteWidth = sizeof(Ox93_TerrainVertex) * 6 * OX93_TERRAIN_NUMSIDEFACES * OX93_TERRAIN_NUMSIDEFACES;
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
	xIndexBufferDesc.ByteWidth = sizeof(u_int) * 6 * OX93_TERRAIN_NUMSIDEFACES * OX93_TERRAIN_NUMSIDEFACES;
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

bool Ox93_Terrain::GenerateRandom(int iTileX, int iTileZ, noise::module::Perlin xModule)
{
	for (u_int uX = 0; uX <= OX93_TERRAIN_NUMSIDEFACES; uX++)
	{
		for (u_int uZ = 0; uZ <= OX93_TERRAIN_NUMSIDEFACES; uZ++)
		{
			// These two are for the height of the tile
			int iX = uX + iTileX * OX93_TERRAIN_NUMSIDEFACES;
			int iZ = uZ + iTileZ * OX93_TERRAIN_NUMSIDEFACES;
			const float fXFrac = (float)iX / (float)OX93_TERRAIN_NUMSIDEFACES;
			const float fZFrac = (float)iZ / (float)OX93_TERRAIN_NUMSIDEFACES;

			const float fBiomeScale = Ox93_Biome::GetTerrainScaleAtPosition(iX, iZ, xModule);
			const float fBiomeAdjust = Ox93_Biome::GetTerrainAdjustAtPosition(iX, iZ, xModule);

			m_afHeightMap[uX][uZ] = (float)xModule.GetValue(fXFrac, fZFrac, OX93_TERRAIN_PERLIN_VALUE) * fBiomeScale + fBiomeAdjust;
		}
	}

	m_xPosition = Ox93_Vector_3(iTileX * OX93_TERRAIN_NUMSIDEFACES * fOX93_TERRAIN_SCALE, 0.f, iTileZ * OX93_TERRAIN_NUMSIDEFACES * fOX93_TERRAIN_SCALE);

	return InitBuffers(Ox93_D3D::GetDevice(), iTileX, iTileZ, xModule);
}

//bool PointsOnSameSideOfLine(Ox93_Vector_3 xP1, Ox93_Vector_3 xP2, Ox93_Vector_3 xA, Ox93_Vector_3 xB)
//{
//	xP1.y = 0.f;
//	xP2.y = 0.f;
//	xA.y = 0.f;
//	xB.y = 0.f;
//
//	Ox93_Vector_3 xCP1 = (xP1 - xA) ^ (xB - xA);
//	Ox93_Vector_3 xCP2 = (xP2 - xA) ^ (xB - xA);
//
//	return (xCP1 * xCP2 >= 0.f);
//}
//
//Ox93_Vector_3 GetClosestPointInTriangle(Ox93_Vector_3 xPos, Ox93_Vector_3 xCorner1, Ox93_Vector_3 xCorner2, Ox93_Vector_3 xCorner3)
//{
//	// Normal
//	Ox93_Vector_3 xNorm = (xCorner3 - xCorner1) ^ (xCorner2 - xCorner1);
//	xNorm.Normalize();
//
//	// Distance
//	float fDist = xNorm * (xPos - xCorner1);
//
//	// Resulting positions
//	Ox93_Vector_3 xPoint = xPos - xNorm * fDist;
//
//	// Check is points are within the triangle
//	bool b1 = PointsOnSameSideOfLine(xPoint, xCorner1, xCorner2, xCorner3);
//	bool b2 = PointsOnSameSideOfLine(xPoint, xCorner2, xCorner3, xCorner1);
//	bool b3 = PointsOnSameSideOfLine(xPoint, xCorner3, xCorner1, xCorner2);
//	if (!b1)
//	{
//		if ((xPoint - xCorner3)*(xCorner2 - xCorner3) > 0.f)
//		{
//			if ((xPoint - xCorner2)*(xCorner3 - xCorner2) > 0.f)
//			{
//				// Needs to be put on edge
//				Ox93_Vector_3 xVec = (xCorner3 - xCorner2);
//				xVec.Normalize();
//				xPoint = xCorner2 + xVec * ((xPoint - xCorner2) * xVec);
//			}
//			else
//			{
//				// Needs to be put on Corner2
//				xPoint = xCorner2;
//			}
//		}
//		else
//		{
//			// Needs to be put on Corner3
//			xPoint = xCorner3;
//		}
//	}
//	else if (!b2)
//	{
//		if ((xPoint - xCorner3)*(xCorner1 - xCorner3) > 0.f)
//		{
//			if ((xPoint - xCorner1)*(xCorner3 - xCorner1) > 0.f)
//			{
//				// Needs to be put on edge
//				Ox93_Vector_3 xVec = (xCorner3 - xCorner1);
//				xVec.Normalize();
//				xPoint = xCorner1 + xVec * ((xPoint - xCorner1) * xVec);
//			}
//			else
//			{
//				// Needs to be put on Corner1
//				xPoint = xCorner1;
//			}
//		}
//		else
//		{
//			// Needs to be put on Corner3
//			xPoint = xCorner3;
//		}
//	}
//	else if (!b3)
//	{
//		if ((xPoint - xCorner2)*(xCorner1 - xCorner2) > 0.f)
//		{
//			if ((xPoint - xCorner1)*(xCorner2 - xCorner1) > 0.f)
//			{
//				// Needs to be put on edge
//				Ox93_Vector_3 xVec = (xCorner2 - xCorner1);
//				xVec.Normalize();
//				xPoint = xCorner1 + xVec * ((xPoint - xCorner1) * xVec);
//			}
//			else
//			{
//				// Needs to be put on Corner1
//				xPoint = xCorner1;
//			}
//		}
//		else
//		{
//			// Needs to be put on Corner2
//			xPoint = xCorner2;
//		}
//	}
//
//	return xPoint;
//}
//
//bool Ox93_Terrain::GetClosestPointInRange(Ox93_Vector_3 xPos, float fRange, Ox93_Vector_3* pxResultPos) const
//{
//	if (!pxResultPos) { return false; }
//	Ox93_Assert(fRange < fOX93_TERRAIN_SCALE, "GetClosestPointInRange is only set up for ranges less than 1 tile width at the moment.");
//	if (fRange >= fOX93_TERRAIN_SCALE) { return false; }
//
//
//	xPos -= m_xPosition;
//
//	int iClosestX = static_cast<int>((xPos.x) / (fOX93_TERRAIN_SCALE * fFrac) + 0.5f);
//	int iClosestZ = static_cast<int>((xPos.z) / fOX93_TERRAIN_SCALE - 0.5f * (iClosestX % 2) + 0.5f);
//
//	if (iClosestX < 0 || iClosestX > OX93_TERRAIN_NUMSIDEFACES) { return false; }
//	if (iClosestZ < 0 || iClosestZ > OX93_TERRAIN_NUMSIDEFACES) { return false; }
//
//	Ox93_Vector_3 xClosestVertex(static_cast<float>(iClosestX) * fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX][iClosestZ], (static_cast<int>(iClosestZ) + 0.5f * (iClosestX % 2)) * fOX93_TERRAIN_SCALE);
//
//	float fDist = fRange + 1.f;
//
//	// Central
//	if (iClosestX > 0 && iClosestZ > 0 && iClosestX < OX93_TERRAIN_NUMSIDEFACES && iClosestZ < OX93_TERRAIN_NUMSIDEFACES)
//	{
//		// Check the intersect point with each of the local triangles
//		Ox93_Vector_3 xCorner[7];
//		xCorner[0] = xClosestVertex;
//		xCorner[1] = Ox93_Vector_3(xClosestVertex.x, m_afHeightMap[iClosestX][iClosestZ - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE);
//		xCorner[2] = Ox93_Vector_3(xClosestVertex.x + fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX + 1][iClosestZ + (iClosestX % 2) - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE / 2.f);
//		xCorner[3] = Ox93_Vector_3(xClosestVertex.x + fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX + 1][iClosestZ + (iClosestX % 2)], xClosestVertex.z + fOX93_TERRAIN_SCALE / 2.f);
//		xCorner[4] = Ox93_Vector_3(xClosestVertex.x, m_afHeightMap[iClosestX][iClosestZ + 1], xClosestVertex.z + fOX93_TERRAIN_SCALE);
//		xCorner[5] = Ox93_Vector_3(xClosestVertex.x - fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX - 1][iClosestZ + (iClosestX % 2)], xClosestVertex.z + fOX93_TERRAIN_SCALE / 2.f);
//		xCorner[6] = Ox93_Vector_3(xClosestVertex.x - fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX - 1][iClosestZ + (iClosestX % 2) - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE / 2.f);
//
//		Ox93_Vector_3 xPoint[6];
//		xPoint[0] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[1], xCorner[2]);
//		xPoint[1] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[2], xCorner[3]);
//		xPoint[2] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[3], xCorner[4]);
//		xPoint[3] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[4], xCorner[5]);
//		xPoint[4] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[5], xCorner[6]);
//		xPoint[5] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[6], xCorner[1]);
//
//		// Find the minimum distance
//		for (int i = 0; i < 6; i++)
//		{
//			if ((xPoint[i] - xPos).Length() < fDist)
//			{
//				*pxResultPos = xPoint[i];
//				fDist = (xPoint[i] - xPos).Length();
//			}
//		}
//	}
//	// neg X edge
//	else if (iClosestZ > 0 && iClosestX < OX93_TERRAIN_NUMSIDEFACES && iClosestZ < OX93_TERRAIN_NUMSIDEFACES)
//	{
//		// Check the intersect point with each of the local triangles
//		Ox93_Vector_3 xCorner[5];
//		xCorner[0] = xClosestVertex;
//		xCorner[1] = Ox93_Vector_3(xClosestVertex.x, m_afHeightMap[iClosestX][iClosestZ - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE);
//		xCorner[2] = Ox93_Vector_3(xClosestVertex.x + fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX + 1][iClosestZ + (iClosestX % 2) - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE / 2.f);
//		xCorner[3] = Ox93_Vector_3(xClosestVertex.x + fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX + 1][iClosestZ + (iClosestX % 2)], xClosestVertex.z + fOX93_TERRAIN_SCALE / 2.f);
//		xCorner[4] = Ox93_Vector_3(xClosestVertex.x, m_afHeightMap[iClosestX][iClosestZ + 1], xClosestVertex.z + fOX93_TERRAIN_SCALE);
//
//		Ox93_Vector_3 xPoint[3];
//		xPoint[0] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[1], xCorner[2]);
//		xPoint[1] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[2], xCorner[3]);
//		xPoint[2] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[3], xCorner[4]);
//
//		// Find the minimum distance
//		for (int i = 0; i < 3; i++)
//		{
//			if ((xPoint[i] - xPos).Length() < fDist)
//			{
//				*pxResultPos = xPoint[i];
//				fDist = (xPoint[i] - xPos).Length();
//			}
//		}
//	}
//	// pos X edge
//	else if (iClosestX > 0 && iClosestZ > 0 && iClosestZ < OX93_TERRAIN_NUMSIDEFACES)
//	{
//		// Check the intersect point with each of the local triangles
//		Ox93_Vector_3 xCorner[5];
//		xCorner[0] = xClosestVertex;
//		xCorner[1] = Ox93_Vector_3(xClosestVertex.x, m_afHeightMap[iClosestX][iClosestZ - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE);
//		xCorner[2] = Ox93_Vector_3(xClosestVertex.x, m_afHeightMap[iClosestX][iClosestZ + 1], xClosestVertex.z + fOX93_TERRAIN_SCALE);
//		xCorner[3] = Ox93_Vector_3(xClosestVertex.x - fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX - 1][iClosestZ + (iClosestX % 2)], xClosestVertex.z + fOX93_TERRAIN_SCALE / 2.f);
//		xCorner[4] = Ox93_Vector_3(xClosestVertex.x - fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX - 1][iClosestZ + (iClosestX % 2) - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE / 2.f);
//
//		Ox93_Vector_3 xPoint[3];
//		xPoint[0] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[2], xCorner[3]);
//		xPoint[1] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[3], xCorner[4]);
//		xPoint[2] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[4], xCorner[1]);
//
//		// Find the minimum distance
//		for (int i = 0; i < 3; i++)
//		{
//			if ((xPoint[i] - xPos).Length() < fDist)
//			{
//				*pxResultPos = xPoint[i];
//				fDist = (xPoint[i] - xPos).Length();
//			}
//		}
//	}
//	// neg Z edge
//	else if ((iClosestX % 2) == 0 && iClosestX > 0 && iClosestX < OX93_TERRAIN_NUMSIDEFACES && iClosestZ < OX93_TERRAIN_NUMSIDEFACES)
//	{
//		// Check the intersect point with each of the local triangles
//		Ox93_Vector_3 xCorner[4];
//		xCorner[0] = xClosestVertex;
//		xCorner[1] = Ox93_Vector_3(xClosestVertex.x + fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX + 1][iClosestZ + (iClosestX % 2)], xClosestVertex.z + fOX93_TERRAIN_SCALE / 2.f);
//		xCorner[2] = Ox93_Vector_3(xClosestVertex.x, m_afHeightMap[iClosestX][iClosestZ + 1], xClosestVertex.z + fOX93_TERRAIN_SCALE);
//		xCorner[3] = Ox93_Vector_3(xClosestVertex.x - fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX - 1][iClosestZ + (iClosestX % 2)], xClosestVertex.z + fOX93_TERRAIN_SCALE / 2.f);
//
//		Ox93_Vector_3 xPoint[2];
//		xPoint[0] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[1], xCorner[3]);
//		xPoint[1] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[3], xCorner[2]);
//
//		// Find the minimum distance
//		for (int i = 0; i < 2; i++)
//		{
//			if ((xPoint[i] - xPos).Length() < fDist)
//			{
//				*pxResultPos = xPoint[i];
//				fDist = (xPoint[i] - xPos).Length();
//			}
//		}
//	}
//	else if ((iClosestX % 2) != 0 && iClosestX > 0 && iClosestX < OX93_TERRAIN_NUMSIDEFACES && iClosestZ < OX93_TERRAIN_NUMSIDEFACES)
//	{
//		// Check the intersect point with each of the local triangles
//		Ox93_Vector_3 xCorner[6];
//		xCorner[0] = xClosestVertex;
//		xCorner[1] = Ox93_Vector_3(xClosestVertex.x + fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX + 1][iClosestZ + (iClosestX % 2) - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE / 2.f);
//		xCorner[2] = Ox93_Vector_3(xClosestVertex.x + fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX + 1][iClosestZ + (iClosestX % 2)], xClosestVertex.z + fOX93_TERRAIN_SCALE / 2.f);
//		xCorner[3] = Ox93_Vector_3(xClosestVertex.x, m_afHeightMap[iClosestX][iClosestZ + 1], xClosestVertex.z + fOX93_TERRAIN_SCALE);
//		xCorner[4] = Ox93_Vector_3(xClosestVertex.x - fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX - 1][iClosestZ + (iClosestX % 2)], xClosestVertex.z + fOX93_TERRAIN_SCALE / 2.f);
//		xCorner[5] = Ox93_Vector_3(xClosestVertex.x - fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX - 1][iClosestZ + (iClosestX % 2) - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE / 2.f);
//
//		Ox93_Vector_3 xPoint[4];
//		xPoint[0] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[1], xCorner[2]);
//		xPoint[1] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[2], xCorner[3]);
//		xPoint[2] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[3], xCorner[4]);
//		xPoint[3] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[4], xCorner[5]);
//
//		// Find the minimum distance
//		for (int i = 0; i < 4; i++)
//		{
//			if ((xPoint[i] - xPos).Length() < fDist)
//			{
//				*pxResultPos = xPoint[i];
//				fDist = (xPoint[i] - xPos).Length();
//			}
//		}
//	}
//	// pos Z edge
//	else if ((iClosestX % 2) == 0 && iClosestX > 0 && iClosestZ > 0 && iClosestX < OX93_TERRAIN_NUMSIDEFACES)
//	{
//		// Check the intersect point with each of the local triangles
//		Ox93_Vector_3 xCorner[6];
//		xCorner[0] = xClosestVertex;
//		xCorner[1] = Ox93_Vector_3(xClosestVertex.x, m_afHeightMap[iClosestX][iClosestZ - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE);
//		xCorner[2] = Ox93_Vector_3(xClosestVertex.x + fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX + 1][iClosestZ + (iClosestX % 2) - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE / 2.f);
//		xCorner[3] = Ox93_Vector_3(xClosestVertex.x + fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX + 1][iClosestZ + (iClosestX % 2)], xClosestVertex.z + fOX93_TERRAIN_SCALE / 2.f);
//		xCorner[4] = Ox93_Vector_3(xClosestVertex.x - fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX - 1][iClosestZ + (iClosestX % 2)], xClosestVertex.z + fOX93_TERRAIN_SCALE / 2.f);
//		xCorner[5] = Ox93_Vector_3(xClosestVertex.x - fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX - 1][iClosestZ + (iClosestX % 2) - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE / 2.f);
//
//		Ox93_Vector_3 xPoint[4];
//		xPoint[0] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[1], xCorner[2]);
//		xPoint[1] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[2], xCorner[3]);
//		xPoint[2] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[4], xCorner[5]);
//		xPoint[3] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[5], xCorner[1]);
//		
//		// Find the minimum distance
//		for (int i = 0; i < 4; i++)
//		{
//			if ((xPoint[i] - xPos).Length() < fDist)
//			{
//				*pxResultPos = xPoint[i];
//				fDist = (xPoint[i] - xPos).Length();
//			}
//		}
//	}
//	else if ((iClosestX % 2) != 0 && iClosestX > 0 && iClosestZ > 0 && iClosestX < OX93_TERRAIN_NUMSIDEFACES)
//	{
//		// Check the intersect point with each of the local triangles
//		Ox93_Vector_3 xCorner[4];
//		xCorner[0] = xClosestVertex;
//		xCorner[1] = Ox93_Vector_3(xClosestVertex.x, m_afHeightMap[iClosestX][iClosestZ - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE);
//		xCorner[2] = Ox93_Vector_3(xClosestVertex.x + fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX + 1][iClosestZ + (iClosestX % 2) - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE / 2.f);
//		xCorner[3] = Ox93_Vector_3(xClosestVertex.x - fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX - 1][iClosestZ + (iClosestX % 2) - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE / 2.f);
//
//		Ox93_Vector_3 xPoint[2];
//		xPoint[0] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[1], xCorner[2]);
//		xPoint[1] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[3], xCorner[1]);
//
//		// Find the minimum distance
//		for (int i = 0; i < 2; i++)
//		{
//			if ((xPoint[i] - xPos).Length() < fDist)
//			{
//				*pxResultPos = xPoint[i];
//				fDist = (xPoint[i] - xPos).Length();
//			}
//		}
//	}
//	// neg X neg Z corner
//	else if (iClosestX < OX93_TERRAIN_NUMSIDEFACES && iClosestZ < OX93_TERRAIN_NUMSIDEFACES)
//	{
//		// Check the intersect point with each of the local triangles
//		Ox93_Vector_3 xCorner[3];
//		xCorner[0] = xClosestVertex;
//		xCorner[1] = Ox93_Vector_3(xClosestVertex.x + fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX + 1][iClosestZ + (iClosestX % 2)], xClosestVertex.z + fOX93_TERRAIN_SCALE / 2.f);
//		xCorner[2] = Ox93_Vector_3(xClosestVertex.x, m_afHeightMap[iClosestX][iClosestZ + 1], xClosestVertex.z + fOX93_TERRAIN_SCALE);
//		
//		Ox93_Vector_3 xPoint[1];
//		xPoint[0] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[1], xCorner[2]);
//		
//		// Find the minimum distance
//		for (int i = 0; i < 1; i++)
//		{
//			if ((xPoint[i] - xPos).Length() < fDist)
//			{
//				*pxResultPos = xPoint[i];
//				fDist = (xPoint[i] - xPos).Length();
//			}
//		}
//	}
//	// neg X pos Z corner
//	else if (iClosestZ > 0 && iClosestX < OX93_TERRAIN_NUMSIDEFACES)
//	{
//		// Check the intersect point with each of the local triangles
//		Ox93_Vector_3 xCorner[4];
//		xCorner[0] = xClosestVertex;
//		xCorner[1] = Ox93_Vector_3(xClosestVertex.x, m_afHeightMap[iClosestX][iClosestZ - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE);
//		xCorner[2] = Ox93_Vector_3(xClosestVertex.x + fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX + 1][iClosestZ + (iClosestX % 2) - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE / 2.f);
//		xCorner[3] = Ox93_Vector_3(xClosestVertex.x + fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX + 1][iClosestZ + (iClosestX % 2)], xClosestVertex.z + fOX93_TERRAIN_SCALE / 2.f);
//		
//		Ox93_Vector_3 xPoint[2];
//		xPoint[0] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[1], xCorner[2]);
//		xPoint[1] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[2], xCorner[3]);
//		
//		// Find the minimum distance
//		for (int i = 0; i < 2; i++)
//		{
//			if ((xPoint[i] - xPos).Length() < fDist)
//			{
//				*pxResultPos = xPoint[i];
//				fDist = (xPoint[i] - xPos).Length();
//			}
//		}
//	}
//	// pos X neg Z corner
//	else if (iClosestX > 0 && iClosestZ < OX93_TERRAIN_NUMSIDEFACES)
//	{
//		// Check the intersect point with each of the local triangles
//		Ox93_Vector_3 xCorner[3];
//		xCorner[0] = xClosestVertex;
//		xCorner[1] = Ox93_Vector_3(xClosestVertex.x, m_afHeightMap[iClosestX][iClosestZ + 1], xClosestVertex.z + fOX93_TERRAIN_SCALE);
//		xCorner[2] = Ox93_Vector_3(xClosestVertex.x - fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX - 1][iClosestZ + (iClosestX % 2)], xClosestVertex.z + fOX93_TERRAIN_SCALE / 2.f);
//		
//		Ox93_Vector_3 xPoint[1];
//		xPoint[0] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[1], xCorner[2]);
//		
//		// Find the minimum distance
//		for (int i = 0; i < 1; i++)
//		{
//			if ((xPoint[i] - xPos).Length() < fDist)
//			{
//				*pxResultPos = xPoint[i];
//				fDist = (xPoint[i] - xPos).Length();
//			}
//		}
//	}
//	// pos X pos Z corner
//	else if (iClosestX > 0 && iClosestZ > 0)
//	{
//		// Check the intersect point with each of the local triangles
//		Ox93_Vector_3 xCorner[4];
//		xCorner[0] = xClosestVertex;
//		xCorner[1] = Ox93_Vector_3(xClosestVertex.x, m_afHeightMap[iClosestX][iClosestZ - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE);
//		xCorner[2] = Ox93_Vector_3(xClosestVertex.x - fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX - 1][iClosestZ + (iClosestX % 2)], xClosestVertex.z + fOX93_TERRAIN_SCALE / 2.f);
//		xCorner[3] = Ox93_Vector_3(xClosestVertex.x - fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iClosestX - 1][iClosestZ + (iClosestX % 2) - 1], xClosestVertex.z - fOX93_TERRAIN_SCALE / 2.f);
//
//		Ox93_Vector_3 xPoint[2];
//		xPoint[0] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[2], xCorner[3]);
//		xPoint[1] = GetClosestPointInTriangle(xPos, xCorner[0], xCorner[3], xCorner[1]);
//		
//		// Find the minimum distance
//		for (int i = 0; i < 2; i++)
//		{
//			if ((xPoint[i] - xPos).Length() < fDist)
//			{
//				*pxResultPos = xPoint[i];
//				fDist = (xPoint[i] - xPos).Length();
//			}
//		}
//	}
//
//
//	*pxResultPos += m_xPosition;
//	return fDist < fRange;
//}
//bool Ox93_Terrain::GetClosestPointToLine(Ox93_Vector_3 xDir, Ox93_Vector_3 xPos, Ox93_Vector_3* pxResult, float* pfDist, float fRange, float fNear /* = 0.f */, bool bForcePositive /* = false */) const
//{
//	if (!pxResult) { return false; }
//
//	float fLowestLineDistSqr = -1.f;
//	Ox93_Vector_3 xResult = xPos;
//
//	for (u_int uX = 0; uX <= OX93_TERRAIN_NUMSIDEFACES; uX++)
//	{
//		for (u_int uZ = 0; uZ <= OX93_TERRAIN_NUMSIDEFACES; uZ++)
//		{
//			const float fX = static_cast<float>(uX) * fFrac * fOX93_TERRAIN_SCALE + m_xPosition.x;
//			const float fZ = (static_cast<float>(uZ) + 0.5f * (uX % 2)) * fOX93_TERRAIN_SCALE + m_xPosition.z;
//
//			const Ox93_Vector_3 xVertexPos(fX, m_afHeightMap[uX][uZ], fZ);
//
//			const Ox93_Vector_3 xDisp = xVertexPos - xPos;
//			const float fDotProd = xDisp * xDir;
//			const float fDispSqr = xDisp * xDisp;
//			const float fLineDistSqr = fDispSqr - fDotProd * fDotProd / (xDir * xDir);
//
//			if( (fLineDistSqr < fLowestLineDistSqr || fLowestLineDistSqr < 0.f)
//				&& fDispSqr < fRange * fRange
//				&& fDispSqr > fNear * fNear
//				&& ( (!bForcePositive) || (fDotProd > 0.f)))
//			{
//				fLowestLineDistSqr = fLineDistSqr;
//				xResult = xVertexPos;
//			}
//		}
//	}
//
//	if (xResult != xPos)
//	{
//		*pxResult = xResult;
//		*pfDist = sqrt(fLowestLineDistSqr);
//		return true;
//	}
//
//	return false;
//}
//
//float Ox93_Terrain::GetHeightAtPoint(Ox93_Vector_3 xPos)
//{
//	int iX = (int)floor((xPos.x - m_xPosition.x) / fOX93_TERRAIN_SCALE);
//	const float fX = static_cast<float>(iX) * fOX93_TERRAIN_SCALE + m_xPosition.x;
//
//	// Adjust position to remove zagged edge effect
//	float fAdjustedPosZ = xPos.z - abs(xPos.x - iX * fOX93_TERRAIN_SCALE - m_xPosition.x);
//
//	int iZ = (int)floor((fAdjustedPosZ - m_xPosition.z) / fOX93_TERRAIN_SCALE);
//	const float fZ = (static_cast<float>(iZ) + 0.5f) * fOX93_TERRAIN_SCALE + m_xPosition.z;
//
//	Ox93_Assert(!(iX < 0 || iX > OX93_TERRAIN_NUMSIDEFACES || iZ < 0 || iZ > OX93_TERRAIN_NUMSIDEFACES), "Input Position out of range for GetHeightAtPoint");
//	if (iX < 0 || iX > OX93_TERRAIN_NUMSIDEFACES) { return 0.f; }
//	if (iZ < 0 || iZ > OX93_TERRAIN_NUMSIDEFACES) { return 0.f; }
//
//
//	const Ox93_Vector_3 xCorner0(fX, m_afHeightMap[iX][iZ], fZ);																					// 0, 0
//	const Ox93_Vector_3 xCorner1(fX + fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iX + 1][iZ], fZ + (0.5f - abs(iX % 2)) * fOX93_TERRAIN_SCALE);		// 1, 0
//	const Ox93_Vector_3 xCorner2(fX, m_afHeightMap[iX][iZ + 1], fZ + fOX93_TERRAIN_SCALE);															// 0, 1
//	const Ox93_Vector_3 xCorner3(fX + fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iX + 1][iZ + 1], fZ + (1.5f - abs(iX % 2)) * fOX93_TERRAIN_SCALE);	// 1, 1
//
//
//	if (iX % 2 == 0)
//	{
//		if (xPos.x < Ox93_Math::Sqrt3 * (fZ - xPos.z) + fX + 2 * fFrac * fOX93_TERRAIN_SCALE)
//		{
//			// 0 1 2
//			Ox93_Vector_3 xNormal = (xCorner2 - xCorner0) ^ (xCorner1 - xCorner0);
//			xNormal.Normalize();
//
//			return (xNormal * xCorner0 - xNormal.x * xPos.x - xNormal.z * xPos.z) / xNormal.y;
//		}
//		else
//		{
//			// 1 2 3
//			Ox93_Vector_3 xNormal = (xCorner2 - xCorner3) ^ (xCorner1 - xCorner3);
//			xNormal.Normalize();
//
//			return (xNormal * xCorner3 - xNormal.x * xPos.x - xNormal.z * xPos.z) / xNormal.y;
//		}
//	}
//	else
//	{
//		if (xPos.x < Ox93_Math::Sqrt3 * (xPos.z - fZ) + fX)
//		{
//			// 0 2 3
//			Ox93_Vector_3 xNormal = (xCorner3 - xCorner0) ^ (xCorner2 - xCorner0);
//			xNormal.Normalize();
//
//			return (xNormal * xCorner0 - xNormal.x * xPos.x - xNormal.z * xPos.z) / xNormal.y;
//		}
//		else
//		{
//			// 0 1 3
//			Ox93_Vector_3 xNormal = (xCorner3 - xCorner0) ^ (xCorner1 - xCorner0);
//			xNormal.Normalize();
//
//			return (xNormal * xCorner0 - xNormal.x * xPos.x - xNormal.z * xPos.z) / xNormal.y;
//		}
//	}
//}
//
//Ox93_Vector_3 Ox93_Terrain::GetClosestCentre(Ox93_Vector_3 xPos)
//{
//	const float fFrac = Ox93_Math::Sqrt3 / 2.f;
//
//	int iX = (int)floor((xPos.x - m_xPosition.x) / (fOX93_TERRAIN_SCALE * fFrac));
//	const float fX = static_cast<float>(iX) * fFrac * fOX93_TERRAIN_SCALE + m_xPosition.x;
//
//	// Adjust position to remove zagged edge effect
//	float fAdjustedPosZ = xPos.z - abs(xPos.x - (iX + (iX % 2)) * fFrac * fOX93_TERRAIN_SCALE - m_xPosition.x) / Ox93_Math::Sqrt3;
//
//	int iZ = (int)floor((fAdjustedPosZ - m_xPosition.z) / fOX93_TERRAIN_SCALE);
//	const float fZ = (static_cast<float>(iZ) + 0.5f * (iX % 2)) * fOX93_TERRAIN_SCALE + m_xPosition.z;
//
//	Ox93_Assert(!(iX < 0 || iX > OX93_TERRAIN_NUMSIDEFACES || iZ < 0 || iZ > OX93_TERRAIN_NUMSIDEFACES), "Input Position out of range for GetHeightAtPoint");
//	if (iX < 0 || iX > OX93_TERRAIN_NUMSIDEFACES) { return Ox93_Math::ZeroVector3; }
//	if (iZ < 0 || iZ > OX93_TERRAIN_NUMSIDEFACES) { return Ox93_Math::ZeroVector3; }
//
//
//	const Ox93_Vector_3 xCorner0(fX, m_afHeightMap[iX][iZ], fZ);																					// 0, 0
//	const Ox93_Vector_3 xCorner1(fX + fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iX + 1][iZ], fZ + (0.5f - abs(iX % 2)) * fOX93_TERRAIN_SCALE);		// 1, 0
//	const Ox93_Vector_3 xCorner2(fX, m_afHeightMap[iX][iZ + 1], fZ + fOX93_TERRAIN_SCALE);															// 0, 1
//	const Ox93_Vector_3 xCorner3(fX + fFrac * fOX93_TERRAIN_SCALE, m_afHeightMap[iX + 1][iZ + 1], fZ + (1.5f - abs(iX % 2)) * fOX93_TERRAIN_SCALE);	// 1, 1
//
//
//	if (iX % 2 == 0)
//	{
//		if (xPos.x < Ox93_Math::Sqrt3 * (fZ - xPos.z) + fX + 2 * fFrac * fOX93_TERRAIN_SCALE)
//		{
//			// 0 1 2
//			Ox93_Vector_3 xReturn = xCorner0 + (xCorner3 - xCorner0) / 3.f;
//			xReturn.y = GetHeightAtPoint(xReturn);
//
//			return xReturn;
//		}
//		else
//		{
//			// 1 2 3
//			Ox93_Vector_3 xReturn = xCorner3 + (xCorner0 - xCorner3) / 3.f;
//			xReturn.y = GetHeightAtPoint(xReturn);
//
//			return xReturn;
//		}
//	}
//	else
//	{
//		if (xPos.x < Ox93_Math::Sqrt3 * (xPos.z - fZ) + fX)
//		{
//			// 0 2 3
//			Ox93_Vector_3 xReturn = xCorner2 + (xCorner1 - xCorner2) / 3.f;
//			xReturn.y = GetHeightAtPoint(xReturn);
//
//			return xReturn;
//		}
//		else
//		{
//			// 0 1 3
//			Ox93_Vector_3 xReturn = xCorner1 + (xCorner2 - xCorner1) / 3.f;
//			xReturn.y = GetHeightAtPoint(xReturn);
//
//			return xReturn;
//		}
//	}
//}

bool Ox93_Terrain::AddPhysicalObject(Ox93_PhysicalObject* pxObject)
{
	if (false && pxObject)
	{
		const float fFrac = Ox93_Math::Sqrt3 / 2.f;

		Ox93_Vector_3 xPos = pxObject->GetPosition();

		int iX = (int)floor((xPos.x - m_xPosition.x) / (fOX93_TERRAIN_SCALE * fFrac));
		const float fX = static_cast<float>(iX) * fFrac * fOX93_TERRAIN_SCALE + m_xPosition.x;

		// Adjust position to remove zagged edge effect
		float fAdjustedPosZ = xPos.z - abs(xPos.x - (iX + (iX % 2)) * fFrac * fOX93_TERRAIN_SCALE - m_xPosition.x) / Ox93_Math::Sqrt3;
		
		int iZ = (int)floor((fAdjustedPosZ - m_xPosition.z) / fOX93_TERRAIN_SCALE);
		const float fZ = (static_cast<float>(iZ) + 0.5f * (iX % 2)) * fOX93_TERRAIN_SCALE + m_xPosition.z;

		Ox93_Assert(!(iX < 0 || iX > OX93_TERRAIN_NUMSIDEFACES || iZ < 0 || iZ > OX93_TERRAIN_NUMSIDEFACES), "Input Position out of range for GetHeightAtPoint");
		if (iX < 0 || iX > OX93_TERRAIN_NUMSIDEFACES) { return 0.f; }
		if (iZ < 0 || iZ > OX93_TERRAIN_NUMSIDEFACES) { return 0.f; }
		

		if (iX % 2 == 0)
		{
			if (xPos.x < Ox93_Math::Sqrt3 * (fZ - xPos.z) + fX + 2 * fFrac * fOX93_TERRAIN_SCALE)
			{
				// 0 1 2
				if (m_apxPhysicalObjects[iX][iZ][0])
				{
					return false;
				}
				else
				{
					m_apxPhysicalObjects[iX][iZ][0] = pxObject;
					return true;
				}
			}
			else
			{
				// 1 2 3
				if (m_apxPhysicalObjects[iX][iZ][1])
				{
					return false;
				}
				else
				{
					m_apxPhysicalObjects[iX][iZ][1] = pxObject;
					return true;
				}
			}
		}
		else
		{
			if (xPos.x < Ox93_Math::Sqrt3 * (xPos.z - fZ) + fX)
			{
				// 0 2 3
				if (m_apxPhysicalObjects[iX][iZ][1])
				{
					return false;
				}
				else
				{
					m_apxPhysicalObjects[iX][iZ][1] = pxObject;
					return true;
				}
			}
			else
			{
				// 0 1 3
				if (m_apxPhysicalObjects[iX][iZ][0])
				{
					return false;
				}
				else
				{
					m_apxPhysicalObjects[iX][iZ][0] = pxObject;
					return true;
				}
			}
		}
	}
	return true;
}