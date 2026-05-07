// strings.h — MSVC compatibility stub
// Ruby 3.2's defines.h includes <strings.h> unconditionally.
// MSVC does not provide this POSIX header, so we stub it here.
// This file must appear in the include path BEFORE Ruby's headers.
#pragma once
#ifndef _STRINGS_H_COMPAT
#define _STRINGS_H_COMPAT
#include <string.h>
#ifdef _MSC_VER
  #define strcasecmp(a,b)    _stricmp(a,b)
  #define strncasecmp(a,b,n) _strnicmp(a,b,n)
#endif
#endif
