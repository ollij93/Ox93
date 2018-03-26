// Includes...
#include "Ox93_Specification.h"
#include "ROOT/Assert/Ox93_Assert.h"
#include "ROOT/Specification/Ox93_Specification_System.h"

Ox93_Specification::Ox93_Specification()
: m_xParameters()
, m_uHash(OX93_HASH_UNSET)
{
}

Ox93_Specification::~Ox93_Specification()
{
}

void Ox93_Specification::Shutdown()
{
	m_xParameters.clear();
}

Ox93_Hash Ox93_Specification::GetValueAsHash(Ox93_Hash uParamHash, Ox93_Hash uDefaultValue) const
{
	const Ox93_Specification_Parameter* pxParam = GetParameterFromHash(uParamHash);
	if (pxParam && pxParam->m_eType == OX93_PARAM_TYPE_HASH)
	{
		return pxParam->m_xValue.uHash;
	}
	else
	{
		return uDefaultValue;
	}
}

int Ox93_Specification::GetValueAsInt(Ox93_Hash uParamHash, int iDefaultValue ) const
{
	const Ox93_Specification_Parameter* pxParam = GetParameterFromHash(uParamHash);
	if (pxParam && pxParam->m_eType == OX93_PARAM_TYPE_INT)
	{
		return pxParam->m_xValue.i;
	}
	else
	{
		return iDefaultValue;
	}
}

u_int Ox93_Specification::GetValueAsUInt(Ox93_Hash uParamHash, u_int uDefaultValue) const
{
	const Ox93_Specification_Parameter* pxParam = GetParameterFromHash(uParamHash);
	if (pxParam && pxParam->m_eType == OX93_PARAM_TYPE_UINT)
	{
		return pxParam->m_xValue.u;
	}
	else
	{
		return uDefaultValue;
	}
}

float Ox93_Specification::GetValueAsFloat(Ox93_Hash uParamHash, float fDefaultValue) const
{
	const Ox93_Specification_Parameter* pxParam = GetParameterFromHash(uParamHash);
	if (pxParam && pxParam->m_eType == OX93_PARAM_TYPE_FLOAT)
	{
		return pxParam->m_xValue.f;
	}
	else
	{
		return fDefaultValue;
	}
}

bool Ox93_Specification::GetValueAsBool(Ox93_Hash uParamHash, bool bDefaultValue) const
{
	const Ox93_Specification_Parameter* pxParam = GetParameterFromHash(uParamHash);
	if (pxParam && pxParam->m_eType == OX93_PARAM_TYPE_BOOL)
	{
		return pxParam->m_xValue.b;
	}
	else
	{
		return bDefaultValue;
	}
}

void Ox93_Specification::LoadParamsFromElement(const tinyxml2::XMLElement* pxElement)
{
	Ox93_Assert(pxElement, "Null element passed to specification for loading.");
	if (pxElement)
	{
		const char* pszName = pxElement->Attribute("name");

		if (m_pszName)
		{
			delete[] m_pszName;
			m_pszName = nullptr;
		}
		m_pszName = new char[strlen(pszName) + 1];
		strcpy_s(m_pszName, (strlen(pszName) + 1)*sizeof(char), pszName);

		if (pszName)
		{
			m_uHash = GetHash(pszName);
			Ox93_Assert(m_uHash != OX93_HASH_UNSET, "Failed to GetHash of specification name.");
			if (m_uHash != OX93_HASH_UNSET)
			{

				const tinyxml2::XMLElement* pxParamElement = pxElement->FirstChildElement("Parameter");
				while (pxParamElement)
				{
					const char* pszParamName = pxParamElement->Attribute("name");

					if (pszParamName)
					{
						Ox93_Specification_Parameter xNewParam;
						xNewParam.m_uHash = GetHash(pszParamName);

						xNewParam.m_eType = static_cast<Ox93_Specification_ParameterType>(pxParamElement->IntAttribute("type"));

						switch (xNewParam.m_eType)
						{
						case OX93_PARAM_TYPE_HASH:
						{
							xNewParam.m_xValue.uHash = GetHash(pxParamElement->Attribute("value"));
							break;
						}
						case OX93_PARAM_TYPE_INT:
						{
							xNewParam.m_xValue.i = pxParamElement->IntAttribute("value");
							break;
						}
						case OX93_PARAM_TYPE_UINT:
						{
							xNewParam.m_xValue.u = pxParamElement->UnsignedAttribute("value");
							break;
						}
						case OX93_PARAM_TYPE_FLOAT:
						{
							xNewParam.m_xValue.f = pxParamElement->FloatAttribute("value");
							break;
						}
						case OX93_PARAM_TYPE_BOOL:
						{
							xNewParam.m_xValue.b = pxParamElement->BoolAttribute("value");
							break;
						}
						default:
							break;

						}


						m_xParameters.push_back(xNewParam);
					}
					pxParamElement = pxParamElement->NextSiblingElement("Parameter");
				}

				// OJ - Successfully read in at least the name hash so add it to the list
				if (!Ox93_Specification_System::GetSpecificationByHash(m_uHash))
				{
					Ox93_Specification_System::AddSpecificationToList(this);
				}
				else
				{
					Ox93_Assert(false, "Attempting to add a specification with an already existing hash. Overrides are not supported.");
				}
			}
		}
	}
}

const Ox93_Specification_Parameter* Ox93_Specification::GetParameterFromHash(Ox93_Hash uParamHash) const
{
	for (u_int u = 0; u < m_xParameters.size(); u++)
	{
		if (m_xParameters[u].m_uHash == uParamHash)
		{
			return &m_xParameters[u];
		}
	}

	return nullptr;
}