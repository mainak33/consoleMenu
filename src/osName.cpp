/*********************************************************************
 * @file  osName.cpp
 *
 * @brief Definitions for members of class OS
 *********************************************************************/

#include "osUtils.h"
#include <stdio.h>

using namespace osUtils;

#if defined(__unix__) || !defined(__APPLE__) && defined(__MACH__)
    #include <sys/param.h>
#endif
#if defined(__APPLE__) && defined(__MACH__) // Apple OSX and iOS (Darwin)
    #include <TargetConditionals.h>
#endif
using namespace osUtils;

// Define the PlatformNames
#if defined(_WIN64)
    OSNameSet OS::platformNames { OS::NAME::WINDOWS64 , OS::NAME::WINDOWS }
#elif defined(_WIN32)
    OSNameSet OS::platformNames{OS::NAME::WINDOWS }
#elif defined(__CYGWIN__) && !defined(_WIN32)
    OSNameSet OS::platformNames{ OS::NAME::WINDOWS }
#elif defined(__ANDROID__)
    OSNameSet OS::platformNames{ OS::NAME::ANDRIOID }
#elif defined(__linux__)
    OSNameSet OS::platformNames{ OS::NAME::LINUX}
#elif defined(__unix__) || !defined(__APPLE__) && defined(__MACH__)
    #if defined(BSD)
        OSNameSet OS::platformNames{ OS::NAME::BSD }
    #else
        OSNameSet OS::platformNames{ OS::NAME::UNIX }
    #endif
#elif defined(__hpux)
    OSNameSet OS::platformNames{ OS::NAME::HP_UX }
#elif defined(_AIX)
    OSNameSet OS::platformNames{ OS::NAME::IBM_AIX }
#elif defined(__APPLE__) && defined(__MACH__) // Apple OSX and iOS (Darwin)
    #if TARGET_IPHONE_SIMULATOR == 1
        OSNameSet OS::platformNames{ OS::NAME::IOS, OS::NAME::APPLE }
    #elif TARGET_OS_IPHONE == 1
        OSNameSet OS::platformNames{ OS::NAME::IOS, OS::NAME::APPLE }
    #elif TARGET_OS_MAC == 1
        OSNameSet OS::platformNames{ OS::NAME::OSX, OS::NAME::APPLE }
    #else
        OSNameSet OS::platformNames{ OS::NAME::APPLE }
    #endif
#elif defined(__sun) && defined(__SVR4)
    OSNameSet OS::platformNames{ OS::NAME::SOLARIS }
#else
    OSNameSet OS::platformNames{}
#endif
;

// Get function instance
OS& OS::instance() {
    static OS osInstance;
    return osInstance;
};

// Check if OS is one of the names
bool OS::is(OS::NAME name) {
    return instance().OS::platformNames.count(name) > 0;
}

// Constructor and destructor
OS::OS() {};
OS::~OS() {};