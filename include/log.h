#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __ANDROID__
#include <android/log.h>
#endif

#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef __ANDROID__

#define debug(fmt, ...)                                                        \
	__android_log_print(ANDROID_LOG_DEBUG, "", "%s#%s:%d "fmt, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define info(fmt, ...)                                                         \
	__android_log_print(ANDROID_LOG_INFO, "", fmt, ##__VA_ARGS__)

#define error(fmt, ...)                                                        \
	__android_log_print(ANDROID_LOG_ERROR, "", fmt, ##__VA_ARGS__)

#else

#ifdef __DEBUG__
#define debug                                                                  \
	printf("%s#%s:%d ", __FILE__, __FUNCTION__, __LINE__);                     \
	printf
#else
#define debug(fmt, ...)
#endif /* __DEBUG__ */

#define info printf

#define error(fmt, ...)                                                        \
	fprintf(stderr, fmt, ##__VA_ARGS__)

#endif /* __ANDROID__ */

#endif /* __DEBUG_H__ */
