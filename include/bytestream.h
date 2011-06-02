#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mfstypes.h>

#ifndef __BYTE_STREAM_H__
#define __BYTE_STREAM_H__

typedef struct ByteStream ByteStream;

#ifdef __cplusplus
extern "C" {
#endif

extern ByteStream* stream_new();

extern size_t stream_get_size(ByteStream *stream);

extern char stream_read(ByteStream *stream);

extern char stream_unread(ByteStream *stream);

extern char stream_read_uint16(ByteStream *stream, uint16_t *value);

extern char stream_read_uint24(ByteStream *stream, uint32_t *value);

extern char stream_read_uint32(ByteStream *stream, uint32_t *value);

extern size_t stream_reads(ByteStream *stream, void *dest, size_t size);

extern size_t stream_unreads(ByteStream *stream, size_t size);

extern void stream_trace(ByteStream *stream);

extern ByteStream* stream_write(ByteStream *stream, unsigned char *data, size_t size);

extern void stream_free(ByteStream **stream);

#ifdef __cplusplus
}
#endif

#endif /* __BYTE_STREAM_H__ */
