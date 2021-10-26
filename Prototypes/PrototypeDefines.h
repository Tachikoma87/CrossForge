/*****************************************************************************\
*                                                                           *
* File(s): Prototype defines.h                                              *
*                                                                           *
* Content: contains global definitions all subfile require.                *
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
#include <string>
#include <vector>
#include <Eigen/Eigen>

#ifdef CFORGE_EXPORTS
#define CFORGE_PROTOTYPE_IXPORT __declspec(dllexport)
#elif defined _WINDLL
#define CFORGE_PROTOTYPE_IXPORT __declspec(dllimport)
#else 
#define CFORGE_PROTOTYPE_IXPORT
#endif