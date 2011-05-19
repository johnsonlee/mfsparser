#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <log.h>
#include <crc.h>
#include <stack.h>
#include <mfsparser.h>

static unsigned char mfsh_mark[] = { 0x00, 0x00, 0x00, 0x01 };

static int find_mux_frame_header(ByteBuffer *buf)
{
	int i = buf->offset;
	for (; i < buf->size && (buf->size - i >= 4); i += 4) {
		if (!memcmp(mfsh_mark, buf->data + i, 4)) {
			debug("find multiplex frame header at %d\n", i);
			debug("ByteBuffer{ data=%p, offset=%d, size=%d }\n", buf->data, buf->offset, buf->size);
			if (!crc32(buf->data + i, buf->data[i + 4] + 4)) {
				return i;
			}
			debug("check crc32 error\n");
		}
	}

	return -1;
}

static int locate_mux_frame_header(ByteBuffer *buf)
{
	int i = -1;

	if (-1 != (i = find_mux_frame_header(buf))) {
		buf->offset = i;
	}

	return i;
}

void parse_mux_frame(ByteBuffer *buf, MFSParserVisitor *visitor)
{
	static MFSParserState state = STATE_NONE;

	switch (state) {
	case STATE_NONE:
	    break;
	default:
	    break;
	}
}

int main(int argc, char **argv)
{
	return 0;
}
