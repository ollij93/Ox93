#pragma once

#ifndef OX93_RANDOM_H__
#define OX93_RANDOM_H__

// Includes...
#include <random>
#include "Ox93_Core.h"

class Ox93_Random
{
public:
	static void Seed(int iSeed) { xGenerator.seed(iSeed); }

	static int GetNewInt(int iLower, int iUpper);
	static float GetNewFloat(float fLower, float fUpper);

protected:
	Ox93_Random() {}
	~Ox93_Random() {}

private:
	static std::default_random_engine xGenerator;
};

#endif // OX93_RANDOM_H__

