#pragma once

#ifndef OX93_SPECIFICATION_SYSTEM_H__
#define OX93_SPECIFICATION_SYSTEM_H__

// Includes...
#include <vector>
#include "Types/Ox93_Hash.h"

// Forward Declarations...
class Ox93_Specification;

class Ox93_Specification_System
{
public:
	static bool Create();
	static void Destroy();

	static void AddSpecificationToList(Ox93_Specification* pxSpec);

	static const Ox93_Specification* GetSpecificationByHash(Ox93_Hash uSpecHash);

protected:
	bool Init();
	void Shutdown();
	void LoadSpecsFromFile(const WCHAR* pwszFilename);

	Ox93_Specification_System();
	~Ox93_Specification_System();

private:
	std::vector<Ox93_Specification*> m_xSpecifications;

	static Ox93_Specification_System* s_pxThis;
};

#endif // OX93_SPECIFICATION_SYSTEM_H__

