#ifndef AUTO_BRIGHTNESS_PROJECT_BMP_CORE_H
#define AUTO_BRIGHTNESS_PROJECT_BMP_CORE_H

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

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
    uint8_t not_important[24];
} __attribute__((packed));
typedef struct DIB_HEADER_ DIB_HEADER;

/// not implemented
typedef char BIT_MASKS;

/// not implemented
typedef char COLOR_TABLE;

struct PIXEL__{
    uint8_t r, g, b, alpha;
};
union PIXEL_{
    struct PIXEL__ components;
    uint32_t value;
    uint8_t bytes[4];
};
typedef union PIXEL_ PIXEL;

struct IMAGE_DATA_{
    uint32_t width, height;
    PIXEL **data;
};
typedef char IMAGE_DATA;

/// not implemented
typedef char COLOR_PROFILE;

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
