// Includes...
#include "Ox93_Random.h"

// Statics...
std::default_random_engine Ox93_Random::xGenerator;

int Ox93_Random::GetNewInt(int iLower, int iUpper)
{
	std::uniform_int_distribution<int> xDistribution(iLower, iUpper);
	return xDistribution(xGenerator);
}

float Ox93_Random::GetNewFloat(float fLower, float fUpper)
{
	std::uniform_real_distribution<float> xDistribution(fLower, fUpper);
	return xDistribution(xGenerator);
}
