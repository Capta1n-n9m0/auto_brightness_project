#ifndef AUTO_BRIGHTNESS_PROJECT_BMP_CORE_H
#define AUTO_BRIGHTNESS_PROJECT_BMP_CORE_H

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_FILENAME_LENGTH 255
extern char *self_executable;

struct BMP_HEADER_{
    uint8_t signature[2];
    uint32_t size;
    uint8_t application_reserved[4];
    uint32_t offset;
} __attribute__((packed));
typedef struct BMP_HEADER_ BMP_HEADER;

struct DIB_HEADER_{
    uint32_t header_size;
    uint32_t width;
    uint32_t height;
    uint16_t n_color_planes;
    uint16_t bits_per_pixels;
    //they are 40, not 24 bytes that we don't use in BITMAPV3INFOHEADER format
    uint8_t not_important[40];
} __attribute__((packed));
typedef struct DIB_HEADER_ DIB_HEADER;

/// not implemented
typedef char BIT_MASKS;

/// not implemented
typedef char COLOR_TABLE;

struct PIXEL__{
    uint8_t r, g, b, a;
} __attribute__((packed));
union PIXEL_{
    struct PIXEL__ components;
    uint8_t bytes[4];
} __attribute__((packed));
typedef union PIXEL_ PIXEL;

struct IMAGE_DATA_{
    uint32_t width, height;
    PIXEL *data;
} __attribute__((packed));
typedef struct IMAGE_DATA_ IMAGE_DATA;

struct IMAGE_DATA_NEW_{
    uint32_t width, height;
    uint8_t bits_per_pixel;
    uint8_t *data;
} __attribute__((packed));
typedef struct IMAGE_DATA_NEW_ IMAGE_DATA_NEW;

/// not implemented
typedef char COLOR_PROFILE;

struct BMP_FILE_{
    char *filename;
    BMP_HEADER *bmpHeader;
    DIB_HEADER *dibHeader;
    BIT_MASKS *bitMasks;
    COLOR_TABLE *colorTable;
    IMAGE_DATA *imageData;
    COLOR_PROFILE *colorProfile;
};
struct BMP_FILE_NEW_{
    char *filename;
    BMP_HEADER *bmpHeader;
    DIB_HEADER *dibHeader;
    BIT_MASKS *bitMasks;
    COLOR_TABLE *colorTable;
    IMAGE_DATA_NEW *imageData;
    COLOR_PROFILE *colorProfile;
};

typedef struct BMP_FILE_ BMP_FILE;
typedef struct BMP_FILE_NEW_ BMP_FILE_NEW;

BMP_FILE *read_bmp_file(FILE *file, bool is_big_endian);
int write_bmp_file(FILE *file, BMP_FILE *data, bool is_big_endian);
BMP_FILE *copy_bmp_structure(const BMP_FILE *input);
void free_bmp_structure(BMP_FILE *bmpFile);


#endif //AUTO_BRIGHTNESS_PROJECT_BMP_CORE_H
