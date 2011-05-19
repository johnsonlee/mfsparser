#include <stddef.h>
#include <stdlib.h>

#ifndef __BYTE_BUFFER_H__
#define __BYTE_BUFFER_H__

typedef struct ByteBuffer             ByteBuffer;

struct ByteBuffer
{
	unsigned char *data;
	unsigned int offset;
	size_t size;
};

#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif

#endif /* __BYTE_BUFFER_H__ */
