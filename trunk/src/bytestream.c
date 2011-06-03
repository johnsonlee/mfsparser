#include <log.h>
#include <bytestream.h>

#undef  __LOG_TAG__
#define __LOG_TAG__ "ByteStream"

struct ByteStream
{
    unsigned char *data;
    unsigned int offset;
    size_t size;
    ByteStream *prev;
    ByteStream *next;
};

static void stream_cleanup(ByteStream *stream)
{
    ByteStream *bs;

    for (bs = stream->next; bs && bs->size <= bs->offset; bs = stream->next) {
        stream_trace(stream);

        stream->next = bs->next;

        if (bs->next) {
            bs->next->prev = NULL;
        } else {
            stream->prev = NULL;
        }

        if (bs->data) {
            debug("free %p\n", bs->data);
            free(bs->data);
            bs->data = NULL;
        }

        free(bs);
        bs = NULL;

        stream_trace(stream);
    }

}

ByteStream* stream_new()
{
    ByteStream *stream = (ByteStream*) malloc(sizeof(ByteStream));

    stream->offset = 0;
    stream->data = NULL;
    stream->size = 0;
    stream->prev = NULL;
    stream->next = NULL;

    return stream;
}

size_t stream_get_size(ByteStream *stream)
{
    size_t size = 0;
    ByteStream *buf;

    for (buf = stream->next; buf; buf = buf->next) {
        size += buf->size - buf->offset;
    }

    return size;
}

ByteStream* stream_write(ByteStream *stream, unsigned char *data, size_t size)
{
    ByteStream *bs = stream_new();
    bs->offset = 0;
    bs->data = malloc(sizeof(unsigned char) * size);
    bs->size = size;

    memcpy(bs->data, data, size);

    if (NULL == stream->prev && NULL == stream->next) {
        stream->next = stream->prev = bs;
        bs->prev = NULL;
    } else {
        stream->prev->next = bs;
        bs->prev = stream->prev;
    }

    bs->next = NULL;
    stream->prev = bs;

    stream_trace(stream);

    return stream;
}

int stream_read(ByteStream *stream)
{
    stream_cleanup(stream);

    ByteStream *buf = stream->next;
    if (buf && buf->data && buf->size > buf->offset) {
        return buf->data[buf->offset++];
    }

    return EOF;
}

int stream_unread(ByteStream *stream)
{
    ByteStream *buf = stream->next;
    if (buf && buf->data && buf->offset > 0) {
        buf->offset--;
        return 1;
    }

    return EOF;
}

int stream_read_uint16(ByteStream *stream, uint16_t *value)
{
    register int i, bval = EOF;

    stream_cleanup(stream);

    *value = 0;
    ByteStream *buf = stream->next;
    if (buf && buf->data && buf->size >= buf->offset + 2) {
        *value  = ((buf->data[buf->offset++] & 0xFF) << 0x08);
        *value |= ((buf->data[buf->offset++] & 0xFF) << 0x00);
        return 2;
    }

    for (i = 1; i >= 0; i--) {
        if (EOF == (bval = stream_read(stream))) {
            return EOF;
        }

        *value |= (bval & 0xFF) << (0x08 * i);
    }

    return 2;
}

int stream_read_uint24(ByteStream *stream, uint32_t *value)
{
    register int i, bval = EOF;

    stream_cleanup(stream);

    *value = 0;
    ByteStream *buf = stream->next;
    if (buf && buf->data && buf->size >= buf->offset + 3) {
        *value  = ((buf->data[buf->offset++] & 0xFF) << 0x10);
        *value |= ((buf->data[buf->offset++] & 0xFF) << 0x08);
        *value |= ((buf->data[buf->offset++] & 0xFF) << 0x00);
        return 3;
    }

    for (i = 2; i >= 0; i--) {
        if (EOF == (bval = stream_read(stream))) {
            return EOF;
        }

        *value |= (bval & 0xFF) << (0x08 * i);
    }

    return 3;
}

int stream_read_uint32(ByteStream *stream, uint32_t *value)
{
    register int i, bval = EOF;

    stream_cleanup(stream);

    *value = 0;
    ByteStream *bs = stream->next;
    if (bs && bs->data && bs->size >= bs->offset + 4) {
        *value  = ((bs->data[bs->offset++] & 0xFF) << 0x18);
        *value |= ((bs->data[bs->offset++] & 0xFF) << 0x10);
        *value |= ((bs->data[bs->offset++] & 0xFF) << 0x08);
        *value |= ((bs->data[bs->offset++] & 0xFF) << 0x00);
        return 4;
    }

    for (i = 3; i >= 0; i--) {
        if (EOF == (bval = stream_read(stream))) {
            return EOF;
        }

        *value |= (bval & 0xFF) << (0x08 * i);
    }

    return 4;
}

size_t stream_reads(ByteStream *stream, void *dest, size_t size)
{
    stream_cleanup(stream);

    ByteStream *bs;
    size_t total = 0, bytes = 0;
    unsigned char *buf = (unsigned char*) dest;

    for (bs = stream->next; bs && total < size; bs = bs->next) {
        if (bs->data && bs->size > bs->offset) {
            bytes = MIN(size - total, bs->size - bs->offset);
            if (NULL != dest) {
                memcpy(buf + total, bs->data + bs->offset, bytes);
            }
#ifdef __DEBUG__
            info("\n********************************* Reading Data *********************************\n");
            register int i;
            for (i = 0; i < bytes; i++) {
                info("%02X", bs->data[bs->offset + i]);
                info("%c", (i + 1) % 27 ? ' ' : '\n');
            }
            info("\n********************************************************************************\n\n");
#endif
            bs->offset += bytes;
            total += bytes;
        }
    }

    return total;
}

size_t stream_unreads(ByteStream *stream, size_t size)
{
    ByteStream *bs = stream->next;
    if (bs && bs->data && bs->offset > 0) {
        size = MIN(bs->offset, size);
        bs->offset -= size;
        return size;
    }

    return EOF;
}

void stream_trace(ByteStream *stream) {
#ifdef __DEBUG__
    ByteStream *buf;

    debug("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Stream Trace >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    for (buf = stream; buf; buf = buf->next) {
        debug("%p <= [%p]{%p|%d,%d} => %p\n", buf->prev, buf, buf->data, buf->offset, buf->size, buf->next);
    }
    debug("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n\n");
#endif
}

void stream_free(ByteStream **stream)
{
    ByteStream *bs;

    stream_trace(*stream);

    for (bs = (*stream)->next; bs; bs = (*stream)->next) {
        (*stream)->next = bs->next;
        if (bs->next) {
            bs->next->prev = bs->next->next ? (*stream) : NULL;
        } else {
            (*stream)->prev = NULL;
        }

        if (bs->data) {
            debug("free %p\n", bs->data);
            free(bs->data);
            bs->data = NULL;
        }
        free(bs);
        bs = NULL;
    }

    if ((*stream)->data) {
        debug("free %p\n", bs->data);
        free((*stream)->data);
        (*stream)->data = NULL;
    }
    free(*stream);
    *stream = NULL;

    stream_trace(*stream);
}
