#ifndef __OX93_ASSERT_H__
#define __OX93_ASSERT_H__

#include "ROOT/Ox93_System.h"

#ifdef USE_OX93_ASSERT
	#define Ox93_Assert(__cond,__hint) \
		do { \
			if(!(__cond)) \
			{ \
				const int iResult = MessageBox(Ox93_System::GetWindow(),L##__hint, L"Assert", MB_ABORTRETRYIGNORE); \
				if(iResult == IDABORT) \
				{ \
					__debugbreak(); \
				} \
			} \
		} while (false)
#else
	#define Ox93_Assert(__cond,__hint) \
		do { (void)sizeof(__cond); } while(false)
#endif

#endif // ifndef __OX93_ASSERT_H__