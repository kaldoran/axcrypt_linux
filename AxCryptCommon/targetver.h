#pragma once

// The following macros define the minimum required platform.  The minimum required platform
// is the earliest version of Windows, Internet Explorer etc. that has the necessary features to run 
// your application.  The macros work by enabling all features available on platform versions up to and 
// including the version specified.

#ifndef WINVER
#define WINVER 0x0501           // Allow use of features specific to Windows XP, Windows Server 2003 or later.
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501     // Allow use of features specific to Windows XP, Windows Server 2003 or later.
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0550        // Specifies that the minimum required platform is Internet Explorer 5.5.
#endif

