#ifndef AUTO_BRIGHTNESS_PROJECT_BMP_CORE_H
#define AUTO_BRIGHTNESS_PROJECT_BMP_CORE_H

#include <inttypes.h>
#include <stdlib.h>

typedef BMP_HEADER;
typedef DIB_HEADER;
typedef BIT_MASKS;
typedef COLOR_TABLE;
typedef IMAGE_DATA;
typedef COLOR_PROFILE;

struct BMP_FILE_{
    BMP_HEADER *bmpHeader;
    DIB_HEADER *dibHeader;
    BIT_MASKS *bitMasks;
    COLOR_TABLE *colorTable;
    IMAGE_DATA *imageData;
    COLOR_PROFILE *colorProfile;
};

typedef struct BMP_FILE_ BMP_FILE;

BMP_FILE *read_bmp_file(const char *filename);
int write_bmp_file(const char *filename, BMP_FILE *data);
void free_bmp_structure(BMP_FILE *bmpFile);



#endif //AUTO_BRIGHTNESS_PROJECT_BMP_CORE_H
