/*****************************************************************************\
*                                                                           *
* File(s):               *
*                                                                           *
* Content:             *
*                                                                           *
*                                                                           *
*                                                                           *
* Author(s): Tom Uhlmann                                                    *
*                                                                           *
*                                                                           *
* The file(s) mentioned above are provided as is under the terms of the     *
* FreeBSD License without any warranty or guaranty to work properly.        *
* For additional license, copyright and contact/support issues see the      *
* supplied documentation.                                                   *
*                                                                           *
\****************************************************************************/

#include <inttypes.h>
#include <vector>
#include <string>

#ifdef CFORGE_EXPORTS
#define CFORGE_API __declspec(dllexport)
#elif defined __WINDLL
#define CFORGE_API __declspec(dllimport)
#else
#define CFORGE_API
#endif