#include "bmp_core.h"

static BMP_HEADER *_read_BMP_HEADER(FILE *f);
static DIB_HEADER *_read_DIB_HEADER(FILE *f);
static IMAGE_DATA *_read_IMAGE_DATA(FILE *f, uint32_t offset, uint32_t width, uint32_t height, bool is_big_endian);

static BMP_HEADER *_read_BMP_HEADER(FILE *f){
    BMP_HEADER *res = calloc(1, sizeof(BMP_HEADER));
    fseek(f, 0, SEEK_SET);

    fread(res, sizeof(BMP_HEADER), 1, f);
#ifdef DEBUG
    fprintf(stderr, "%s: [ DEBUG ] Reading BMP header\n", self_executable);
    fprintf(stderr, "%s: [ DEBUG ] Signature: %c%c\n", self_executable, res->signature[0], res->signature[1]);
    fprintf(stderr, "%s: [ DEBUG ] Size: %d\n", self_executable, res->size);
    fprintf(stderr, "%s: [ DEBUG ] Application bytes: 0x", self_executable);
    for(int i = 0; i < 4; i++){
        fprintf(stderr, "%02X", res->application_reserved[i]);
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "%s: [ DEBUG ] Offset: %d\n", self_executable, res->offset);
    fprintf(stderr, "\n");
#endif

    return res;
}
static DIB_HEADER *_read_DIB_HEADER(FILE *f){
    DIB_HEADER *res = calloc(1, sizeof(DIB_HEADER));
    fseek(f, sizeof(BMP_HEADER), SEEK_SET);

    fread(res, sizeof(DIB_HEADER), 1, f);
#ifdef DEBUG
    fprintf(stderr, "%s: [ DEBUG ] Reading DIB header\n", self_executable);
    fprintf(stderr, "%s: [ DEBUG ] Header size: %d\n", self_executable, res->header_size);
    fprintf(stderr, "%s: [ DEBUG ] Image width: %d\n", self_executable, res->width);
    fprintf(stderr, "%s: [ DEBUG ] Image height: %d\n", self_executable, res->height);
    fprintf(stderr, "%s: [ DEBUG ] Color planes: %d\n", self_executable, res->n_color_planes);
    fprintf(stderr, "%s: [ DEBUG ] Bits per pixel: %d\n", self_executable, res->bits_per_pixels);
    fprintf(stderr, "%s: [ DEBUG ] Other bytes: 0x", self_executable);
    for(int i = 0; i < 24; i++){
        if(i%4 == 0 && i != 0) printf(" ");
        fprintf(stderr, "%02X", res->not_important[i]);
    }
    fprintf(stderr, "\n\n");
#endif

    return res;
}
static IMAGE_DATA *_read_IMAGE_DATA(FILE *f, uint32_t offset, uint32_t width, uint32_t height, bool is_big_endian) {
    IMAGE_DATA *res = calloc(sizeof(IMAGE_DATA), 1);
    fseek(f, offset, SEEK_SET);

    res->width = width;
    res->height = height;
    res->data = calloc(sizeof(PIXEL), width*height);
    if(is_big_endian) fread(res->data, sizeof(PIXEL), width*height, f);
    else{
        for(int i = 0; i < width*height; i++){
            fread(&res->data[i].bytes[3], sizeof(uint8_t), 1, f);
            fread(&res->data[i].bytes[2], sizeof(uint8_t), 1, f);
            fread(&res->data[i].bytes[1], sizeof(uint8_t), 1, f);
            fread(&res->data[i].bytes[0], sizeof(uint8_t), 1, f);
        }
    }

#ifdef DEBUG
    fprintf(stderr, "%s: [ DEBUG ] Reading image data\n", self_executable);
    fprintf(stderr, "%s: [ DEBUG ] Pixels:\n", self_executable);
    for(int i = 0; i < 5; i++){
        int n = i;
        PIXEL p = ((PIXEL *)res->data)[n];
        fprintf(stderr, "%d:{r:%d,g:%d,b:%d,a:%d}\n", n, p.components.r, p.components.g, p.components.b, p.components.a);
    }
    fprintf(stderr, "\n...\n\n");
    for(int i = 0; i < 5; i++){
        int n = width*height - i - 1;
        PIXEL p = ((PIXEL *)res->data)[n];
        fprintf(stderr, "%d:{r:%d,g:%d,b:%d,a:%d}\n", n, p.components.r, p.components.g, p.components.b, p.components.a);
    }
    fprintf(stderr, "\n");
#endif

    return res;
}

BMP_FILE *read_bmp_file(const char *filename, bool is_big_endian) {
    BMP_FILE *result = calloc(1, sizeof(BMP_FILE));
    FILE *file = fopen(filename, "rb");
    if(file == NULL) {
        fprintf(stderr, "%s: [ FATAL ] Failed to open %s\n", self_executable, filename);
        exit(2);
    }
    result->bmpHeader = _read_BMP_HEADER(file);
    result->dibHeader = _read_DIB_HEADER(file);
    result->imageData = _read_IMAGE_DATA(file, result->bmpHeader->offset, result->dibHeader->width,
                                         result->dibHeader->height, is_big_endian);

    fclose(file);
    return result;
}
int write_bmp_file(const char *filename, BMP_FILE *data, bool is_big_endian) {
#ifdef DEBUG
    fprintf(stderr, "%s: [ DEBUG ] Writing %s file.\n", self_executable, filename);
#endif
    FILE *f = fopen(filename, "wb");
    if(f == NULL){
        fprintf(stderr, "%s: [ FATAL ] Failed to open %s\n", self_executable, filename);
        exit(2);
    }
    uint8_t zero = 0;
    uint32_t width = data->dibHeader->width;
    uint32_t height = data->dibHeader->height;

#ifdef DEBUG
    fprintf(stderr, "%s: [ DEBUG ] Writing BMP header.\n", self_executable);
#endif
    fwrite(data->bmpHeader, sizeof(BMP_HEADER), 1, f);
#ifdef DEBUG
    fprintf(stderr, "%s: [ DEBUG ] Writing DIB header.\n", self_executable);
#endif
    fwrite(data->dibHeader, sizeof(DIB_HEADER), 1, f);
#ifdef DEBUG
    fprintf(stderr, "%s: [ DEBUG ] Writing padding.\n", self_executable);
#endif
    fwrite(&zero, sizeof(uint8_t), data->bmpHeader->offset-(sizeof(BMP_HEADER) + sizeof(DIB_HEADER)), f);

#ifdef DEBUG
    fprintf(stderr, "%s: [ DEBUG ] Writing pixel array in ", self_executable);
    if(is_big_endian) fprintf(stderr, "big");
    else fprintf(stderr, "little");
    fprintf(stderr, " endian: \n");
#endif
    if(is_big_endian) fwrite(data->imageData->data, sizeof(PIXEL), width*height ,f);
    else{
        bool dots_condition = true;
        for(int i = 0; i < width*height; i++){
#ifdef DEBUG
            if(i < 5 || i >= width*height-5){
                uint8_t r = data->imageData->data[i].components.r;
                uint8_t g = data->imageData->data[i].components.g;
                uint8_t b = data->imageData->data[i].components.b;
                uint8_t a = data->imageData->data[i].components.a;
                fprintf(stderr, "%d:{r:%d,g:%d,b:%d,a:%d}\n", i, r, g, b, a);
            } else{
                if(dots_condition) fprintf(stderr, "\n...\n");
                dots_condition = false;
            }
#endif
            fwrite(&data->imageData->data[i].bytes[3], sizeof(uint8_t), 1, f);
            fwrite(&data->imageData->data[i].bytes[2], sizeof(uint8_t), 1, f);
            fwrite(&data->imageData->data[i].bytes[1], sizeof(uint8_t), 1, f);
            fwrite(&data->imageData->data[i].bytes[0], sizeof(uint8_t), 1, f);
        }
    }
    uint32_t file_length = (data->dibHeader->width * data->dibHeader->height + data->bmpHeader->offset);
    uint8_t padding = 0;
    if(file_length % 4) padding = 4 - (file_length%4);
    if(padding) fwrite(&zero, sizeof(uint8_t), padding, f);
    fclose(f);
    return 0;
}
BMP_FILE *copy_bmp_structure(const BMP_FILE *input){
    BMP_FILE *res = calloc(1, sizeof(BMP_FILE));

    res->bmpHeader = calloc(1, sizeof(BMP_HEADER));
    *res->bmpHeader = *input->bmpHeader;
    res->dibHeader = calloc(1, sizeof(DIB_HEADER));
    *res->dibHeader = *input->dibHeader;
    uint32_t width, height;
    width = res->dibHeader->width;
    height = res->dibHeader->height;
    res->imageData = calloc(1, sizeof(IMAGE_DATA));
    res->imageData->width = width;
    res->imageData->height = height;
    res->imageData->data = calloc(width*height, sizeof(PIXEL));
    memcpy(res->imageData->data, input->imageData->data, width*height*sizeof(PIXEL));

    return res;
}
void free_bmp_structure(BMP_FILE *bmpFile){
    if(bmpFile->bmpHeader) free(bmpFile->bmpHeader);
    if(bmpFile->dibHeader) free(bmpFile->dibHeader);
    if(bmpFile->bitMasks) free(bmpFile->bitMasks);
    if(bmpFile->colorTable) free(bmpFile->colorTable);
    if(bmpFile->imageData) free(bmpFile->imageData);
    if(bmpFile->colorProfile) free(bmpFile->imageData);
    free(bmpFile);
}
