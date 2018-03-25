#pragma once

#ifndef __OX93_BIOME_H__
#define __OX93_BIOME_H__

#include "Ox93_Core.h"
#include "LibNoise/noise.h"

#define OX93_BIOME_PERLIN_VALUE 1.5f
#define OX93_BIOME_SCALE 128.f

namespace Ox93_Biome {
	enum Ox93_Biome {
		DEFAULT = 0,
		MOUNTAIN,
		HILLS,
	};

	struct BiomeRegister {
		Ox93_Biome eClass;
		float fWeight;
		Ox93_Color xTerrainColor;
		// The following should always be functions that are continuous with the neighbouring biomes
		float(*pfnScale)(float f); // The scale of the terrain noise in this biome
		float (*pfnAdjust)(float f); // The offset for the average position in this biome
	};

	// TODO-OJ : This needs a better system to compensate for neighbors and allow any biome to surround any other biome
	const static BiomeRegister axBiomeRegistry[] = {
		{ MOUNTAIN, -0.7f, { 0.5f, 0.5f, 0.5f, 1.f },
			[](float f) -> float { return 10.f + 40.f * (-0.4f - f) + 150.f * (-0.7f - f); },
			[](float f) -> float { return 40.f * (-0.4f - f) + 150.f * (-0.7f - f); },
		},
		{ HILLS,  -0.4f, { 0.3f, 0.4f, 0.3f, 1.f },
			[](float f) -> float { return 10.f + 40.f * (-0.4f - f); },
			[](float f) -> float { return 40.f * (-0.4f - f); },
		},
		{ DEFAULT, 2.0f, { 0.3f, 0.6f, 0.3f, 1.f },
			[](float f) -> float { return 10.f; },
			[](float f) -> float { return 0.f; },
		},
	};

	inline BiomeRegister GetBiomeRegisterFromValue(float fValue) {
		for (int i = 0; i < sizeof(axBiomeRegistry) / sizeof(BiomeRegister); i++) {
			if (axBiomeRegistry[i].fWeight > fValue)
				return axBiomeRegistry[i];
		}
		Ox93_Assert(false, "No Biome registered for given value.");
		return BiomeRegister();
	}
	inline BiomeRegister GetBiomeRegisterAtPosition(int iX, int iZ, const noise::module::Perlin& xModule) {
		return GetBiomeRegisterFromValue((float)xModule.GetValue(iX / (float)OX93_BIOME_SCALE, iZ / (float)OX93_BIOME_SCALE, OX93_BIOME_PERLIN_VALUE));
	}

	Ox93_Color GetTerrainColorAtPosition(int iX, int iZ, const noise::module::Perlin& xModule) {
		return GetBiomeRegisterAtPosition(iX, iZ, xModule).xTerrainColor;
	}
	Ox93_Biome GetBiomeAtPosition(int iX, int iZ, const noise::module::Perlin& xModule) {
		return GetBiomeRegisterAtPosition(iX, iZ, xModule).eClass;
	}
	float GetTerrainScaleAtPosition(int iX, int iZ, const noise::module::Perlin& xModule) {
		return GetBiomeRegisterAtPosition(iX, iZ, xModule).pfnScale((float)xModule.GetValue(iX / (float)OX93_BIOME_SCALE, iZ / (float)OX93_BIOME_SCALE, OX93_BIOME_PERLIN_VALUE));
	}
	float GetTerrainAdjustAtPosition(int iX, int iZ, const noise::module::Perlin& xModule) {
		return GetBiomeRegisterAtPosition(iX, iZ, xModule).pfnAdjust((float)xModule.GetValue(iX / (float)OX93_BIOME_SCALE, iZ / (float)OX93_BIOME_SCALE, OX93_BIOME_PERLIN_VALUE));
	}
}

#endif // __OX93_BIOME_H__
