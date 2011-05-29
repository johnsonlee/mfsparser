#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __ANDROID__
#include <android/log.h>
#endif

#ifndef __LOG_H__
#define __LOG_H__

#ifndef __LOG_TAG__
#define __LOG_TAG__ ""
#endif

#if defined(__ANDROID__)
#  define debug(fmt, ...)                                                      \
     __android_log_print(ANDROID_LOG_DEBUG, __LOG_TAG__, "%s:%d "fmt,          \
             __FUNCTION__, __LINE__, ##__VA_ARGS__)
#  define info(fmt, ...)                                                       \
     __android_log_print(ANDROID_LOG_INFO, __LOG_TAG__, fmt, ##__VA_ARGS__)
#  define warn(fmt, ...)                                                       \
     __android_log_print(ANDROID_LOG_WARN, __LOG_TAG__, fmt, ##__VA_ARGS__)
#  define error(fmt, ...)                                                      \
     __android_log_print(ANDROID_LOG_ERROR, __LOG_TAG__, fmt, ##__VA_ARGS__)
#elif defined(__X86__)
#  if defined(__DEBUG_BY_STEP__)
#    define debug getchar(); printf("%s#%s:%d ", __FILE__, __FUNCTION__, __LINE__); printf
#  else
#    define debug printf("%s#%s:%d ", __FILE__, __FUNCTION__, __LINE__); printf
#  endif
#  define warn(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#  define info printf
#  define error(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#  define debug(fmt, ...)
#  define info(fmt, ...)
#  define warn(fmt, ...)
#  define error(fmt, ...)
#endif

#endif /* __LOG_H__ */
