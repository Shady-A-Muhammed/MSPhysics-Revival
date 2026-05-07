// MSPhysics Revival - Updated for Ruby 3.2 (SketchUp 2024+)
// Original by Thomthom/TT_lib2, extended by MSPhysics team
// Updated 2026: removed Ruby 1.8/2.0 legacy, added Ruby 3.x support

#ifndef RUBY_PREP_H
#define RUBY_PREP_H

// -----------------------------------------------------------------------
// Platform detection
// -----------------------------------------------------------------------
#ifdef _MSC_VER
    #ifdef _M_X64
        #define _WIN_64_VER
    #else
        #define _WIN_32_VER
    #endif
#else
    #define _MACOSX_VER
#endif

// -----------------------------------------------------------------------
// SIMD headers
// -----------------------------------------------------------------------
#if defined(_WIN_32_VER) || defined(_WIN_64_VER)
    #include <intrin.h>
    #include <emmintrin.h>
#endif

#ifdef _MACOSX_VER
    #include <unistd.h>
    #include <sys/sysctl.h>
    #include <assert.h>
    #if defined(__i386__) || defined(__x86_64__)
        #include <fenv.h>
        #include <pmmintrin.h>
        #include <emmintrin.h>
        #include <mmintrin.h>
    #endif
#endif

// -----------------------------------------------------------------------
// Ruby 3.x MSVC configuration
// Ruby in SketchUp 2024+ is built with /MD. These defines ensure our
// extension links against the same runtime.
// -----------------------------------------------------------------------
#if _MT
    #if _MSC_VER >= 1800  // VS 2013+
        #define HAVE_ACOSH 1
        #define HAVE_CBRT  1
        #define HAVE_ERF   1
        #define HAVE_TGAMMA 1
        #define HAVE_ROUND 1
        #define HAVE_NEXTAFTER 1
    #endif

    #if _MSC_VER >= 1900  // VS 2015+
        #ifndef HAVE_STRUCT_TIMESPEC
            #define HAVE_STRUCT_TIMESPEC 1
        #endif
    #endif
#endif
// -----------------------------------------------------------------------
// strings.h stub for MSVC (Ruby 3.2 headers require this POSIX header)
// -----------------------------------------------------------------------
#ifdef _MSC_VER
  #include <string.h>
  #ifndef _STRINGS_H
    #define _STRINGS_H
    #define strcasecmp  _stricmp
    #define strncasecmp _strnicmp
  #endif
#endif
// -----------------------------------------------------------------------
// Ruby Headers
// Winsock must come BEFORE ruby.h to avoid winsock/winsock2 conflict
// -----------------------------------------------------------------------
#if defined(_WIN_32_VER) || defined(_WIN_64_VER)
    #define NOMINMAX        // prevent windows.h min/max macro conflicts
    #include <Winsock2.h>
#endif

#include <ruby.h>
#include <ruby/encoding.h>

// -----------------------------------------------------------------------
// Ruby 3.2 API compatibility
// rb_str_new2 was removed in Ruby 3.x — use rb_utf8_str_new_cstr instead.
// Provide a shim here as a safety net in case any file was missed.
// -----------------------------------------------------------------------
#ifndef rb_str_new2
    #define rb_str_new2(str) rb_utf8_str_new_cstr(str)
#endif

// -----------------------------------------------------------------------
// Compatibility macros (Ruby 2.x still needed for some patterns)
// -----------------------------------------------------------------------
#ifndef RARRAY_PTR
    #define RARRAY_PTR(s) (RARRAY(s)->ptr)
#endif

#ifndef RARRAY_LEN
    #define RARRAY_LEN(x) (RARRAY(x)->len)
#endif

#ifndef RSTRING_PTR
    #define RSTRING_PTR(s) (RSTRING(s)->ptr)
#endif

#ifndef RSTRING_LEN
    #define RSTRING_LEN(s) (RSTRING(s)->len)
#endif

#ifndef DBL2NUM
    #define DBL2NUM(dbl) rb_float_new(dbl)
#endif

#ifndef NUM2SIZET
    #if defined(HAVE_LONG_LONG) && SIZEOF_SIZE_T > SIZEOF_LONG
        #define NUM2SIZET(x) ((size_t)NUM2ULL(x))
    #else
        #define NUM2SIZET(x) NUM2ULONG(x)
    #endif
#endif

// -----------------------------------------------------------------------
// Function pointer casts for rb_define_method etc.
// Ruby 3.x tightened these — VALUEFUNC/VOIDFUNC must be exact.
// -----------------------------------------------------------------------
#define INTFUNC(f)    ((int (*)(ANYARGS)) f)
#define VALUEFUNC(f)  ((VALUE (*)(ANYARGS)) f)
#define VOIDFUNC(f)   ((RUBY_DATA_FUNC) f)

// -----------------------------------------------------------------------
// Undefine macros that Ruby/win32.h defines and that conflict with C++ STL
// -----------------------------------------------------------------------
#ifdef getc
    #undef getc
#endif
#ifdef putc
    #undef putc
#endif
#ifdef fgetc
    #undef fgetc
#endif
#ifdef fputc
    #undef fputc
#endif
#ifdef getchar
    #undef getchar
#endif
#ifdef putchar
    #undef putchar
#endif
#ifdef fgetchar
    #undef fgetchar
#endif
#ifdef fputchar
    #undef fputchar
#endif
#ifdef utime
    #undef utime
#endif
#ifdef close
    #undef close
#endif
#ifdef fclose
    #undef fclose
#endif
#ifdef read
    #undef read
#endif
#ifdef write
    #undef write
#endif
#ifdef getpid
    #undef getpid
#endif
#ifdef sleep
    #undef sleep
#endif
#ifdef connect
    #undef connect
#endif
#ifdef mode_t
    #undef mode_t
#endif
#ifdef inline
    #undef inline
#endif

#endif /* RUBY_PREP_H */
