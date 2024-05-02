#ifndef MTC_COREGLOBAL_H
#define MTC_COREGLOBAL_H

#ifdef _MSC_VER
#  define MTC_DECL_EXPORT __declspec(dllexport)
#  define MTC_DECL_IMPORT __declspec(dllimport)
#else
#  define MTC_DECL_EXPORT __attribute__((visibility("default")))
#  define MTC_DECL_IMPORT __attribute__((visibility("default")))
#endif

#ifndef MTC_CORE_EXPORT
#  ifdef MTC_CORE_STATIC
#    define MTC_CORE_EXPORT
#  else
#    ifdef MTC_CORE_LIBRARY
#      define MTC_CORE_EXPORT MTC_DECL_EXPORT
#    else
#      define MTC_CORE_EXPORT MTC_DECL_IMPORT
#    endif
#  endif
#endif

#endif // MTC_COREGLOBAL_H
