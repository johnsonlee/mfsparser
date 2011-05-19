#include <stdlib.h>

#ifndef __CRC_H__
#define __CRC_H__

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned long crc32(unsigned char *data, unsigned long length);

extern unsigned long crc8(unsigned char *data, unsigned long length);

#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */
