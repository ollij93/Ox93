#pragma once

#ifndef OX93_HASH_H__
#define OX93_HASH_H__

#include <stdint.h>
#include <Windows.h>

typedef unsigned long u_long;
typedef uint32_t u_int;
typedef u_int Ox93_Hash;

//////////////////////////////////////
// G L O B A L   V A R I A B L E S
//////////////////////////////////////
const Ox93_Hash OX93_HASH_UNSET = 0;

//////////////////////////////////////
// G L O B A L   F U N C T I O N S
//////////////////////////////////////
Ox93_Hash GetHash(const char* pszString);
Ox93_Hash GetHash(const WCHAR* pwszString);

#endif // OX93_HASH_H__