#include "Ox93_Hash.h"

Ox93_Hash GetHash(const char* pszString)
{
	if (!pszString) { return 0; }

	Ox93_Hash uHash = OX93_HASH_UNSET;
	for (u_int u = 0; pszString[u]; u++)
	{
		Ox93_Assert(u <= 256, "Attempting to get too long a hash.");
		if (u > 256)
		{
			return OX93_HASH_UNSET;
		}
		uHash *= 31;
		uHash += pszString[u];
	}

	return uHash;
}

Ox93_Hash GetHash(const WCHAR* pwszString)
{
	if (!pwszString) { return 0; }

	Ox93_Hash uHash = OX93_HASH_UNSET;
	for (u_int u = 0; pwszString[u]; u++)
	{
		Ox93_Assert(u <= 256, "Attempting to get too long a hash.");
		if (u > 256)
		{
			return OX93_HASH_UNSET;
		}

		Ox93_Assert(0 <= pwszString[u] && pwszString[u] < 256, "Attempting to get hash for string containing unexpected symbol. \n Unexpected results will occur.");

		uHash *= 31;
		uHash += static_cast<char>(pwszString[u]);
	}

	return uHash;
}