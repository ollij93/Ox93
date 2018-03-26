#pragma once

#ifndef OX93_SPECIFICATION_H__
#define OX93_SPECIFICATION_H__

// Includes...
#include <vector>
#include "TinyXML/tinyxml2.h"
#include "Types/Ox93_Value.h"

enum Ox93_Specification_ParameterType
{
	OX93_PARAM_TYPE_HASH = 0,
	OX93_PARAM_TYPE_INT = 1,
	OX93_PARAM_TYPE_UINT = 2,
	OX93_PARAM_TYPE_FLOAT = 3,
	OX93_PARAM_TYPE_BOOL = 4,
};

struct Ox93_Specification_Parameter
{
	Ox93_Hash m_uHash;
	Ox93_Specification_ParameterType m_eType;
	Ox93_Value m_xValue;
};

class Ox93_Specification
{
	friend class Ox93_Specification_System;

public:
	Ox93_Specification();
	~Ox93_Specification();

	void Shutdown();

	Ox93_Hash GetValueAsHash(Ox93_Hash uParameterHash, Ox93_Hash uDefaultValue) const;
	int GetValueAsInt(Ox93_Hash uParameterHash, int iDefaultValue) const;
	u_int GetValueAsUInt(Ox93_Hash uParameterHash, u_int uDefaultValue) const;
	float GetValueAsFloat(Ox93_Hash uParameterHash, float fDefaultValue) const;
	bool GetValueAsBool(Ox93_Hash uParameterHash, bool bDefaultValue) const;
	const char* GetName() const { return m_pszName; }

	Ox93_Hash GetHashID() const { return m_uHash; }

protected:
	void LoadParamsFromElement(const tinyxml2::XMLElement* pxElement);

private:
	const Ox93_Specification_Parameter* GetParameterFromHash(Ox93_Hash uParamHash) const;

	std::vector<Ox93_Specification_Parameter> m_xParameters;
	Ox93_Hash m_uHash;
	char* m_pszName;
};

#endif // OX93_SPECIFICATION_H__
