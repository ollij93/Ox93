#pragma once

#ifndef OX93_VALUE_H__
#define OX93_VALUE_H__

#include "Ox93_Hash.h"

union Ox93_Value 
{
	Ox93_Hash uHash;
	int i;
	u_int u;
	float f;
	bool b;
};

#endif // OX93_VALUE_H__

