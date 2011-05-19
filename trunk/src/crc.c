#include <crc.h>

#define CHECK_CRC_32_TABLE_INIT()                                              \
	do {                                                                       \
		if (!init) crc_32_table_init();                                        \
	} while (0)

static char init = 0;

static unsigned long crc_32_table[256];

static void crc_32_table_init()
{
	unsigned long i, j, data, accum, polynomial = 0x04C11DB7;

	for (i = 0; i < 256; i++) {
		data = (unsigned long) (i << 24);

		for (j = 0, accum = 0; j < 8; j++) {
			if ((data ^ accum) & 0x80000000) {
				accum = (accum << 1) ^ polynomial;
			} else {
				accum <<= 1;
			}
			data <<= 1;
		}
		crc_32_table[i] = accum;
	}

	init = 1;
}

unsigned long crc32(unsigned char *data, unsigned long length)
{
	CHECK_CRC_32_TABLE_INIT();

	unsigned long i, accum = -1;

	for (i = 0; i < length; i++) {
		accum = (accum << 8) ^ crc_32_table[(accum >> 24) ^ *data++];
	}

	return accum;
}

unsigned long crc8(unsigned char *data, unsigned long length)
{
	CHECK_CRC_32_TABLE_INIT();

	unsigned int i;
	unsigned char crc = 0;

	while (length--) {
		crc ^= *data++;
		for (i = 0; i < 8; i++) {
			if (crc & 0x80) {
				crc = (crc << 1) ^ 0x31;
			} else {
				crc <<= 1;
			}
		}
	}

	return crc;
}
