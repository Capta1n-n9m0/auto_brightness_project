#include "bmp_core.h"

static BMP_HEADER *_read_BMP_HEADER(FILE *f);
static DIB_HEADER *_read_DIB_HEADER(FILE *f);
static IMAGE_DATA *_read_IMAGE_DATA(FILE *f, uint32_t offset, uint32_t width, uint32_t height);

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
static IMAGE_DATA *_read_IMAGE_DATA(FILE *f, uint32_t offset, uint32_t width, uint32_t height){
    IMAGE_DATA *res = calloc(sizeof(IMAGE_DATA), 1);
    fseek(f, offset, SEEK_SET);

    res->width = width;
    res->height = height;
    res->data = calloc(sizeof(PIXEL), width*height);
    fread(res->data, sizeof(PIXEL), width*height, f);

#ifdef DEBUG
    fprintf(stderr, "%s: [ DEBUG ] Reading image data\n", self_executable);
    fprintf(stderr, "%s: [ DEBUG ] Pixels:\n", self_executable);
    for(int i = 0; i < 5; i++){
        int n = i;
        PIXEL p = ((PIXEL *)res->data)[n];
        fprintf(stderr, "%d:{r:%d,g:%d,b:%d,a:%d}\n", n, p.components.r, p.components.g, p.components.b, p.components.a);
    }
    fprintf(stderr, "\n...\n");
    for(int i = 0; i < 5; i++){
        int n = width*height - i - 1;
        PIXEL p = ((PIXEL *)res->data)[n];
        fprintf(stderr, "%d:{r:%d,g:%d,b:%d,a:%d}\n", n, p.components.r, p.components.g, p.components.b, p.components.a);
    }
    fprintf(stderr, "\n");
#endif

    return res;
}

BMP_FILE *read_bmp_file(const char *filename){
    BMP_FILE *result = calloc(1, sizeof(BMP_FILE));
    FILE *file = fopen(filename, "rb");
    if(file == NULL) {
        fprintf(stderr, "%s: [ FATAL ] Failed to open %s\n", self_executable, filename);
        exit(2);
    }
    result->bmpHeader = _read_BMP_HEADER(file);
    result->dibHeader = _read_DIB_HEADER(file);
    result->imageData = _read_IMAGE_DATA(file, result->bmpHeader->offset, result->dibHeader->width, result->dibHeader->height);

    fclose(file);
    return result;
}
int write_bmp_file(const char *filename, BMP_FILE *data){
    FILE *f = fopen(filename, "wb");
    if(f == NULL){
        fprintf(stderr, "%s: [ FATAL ] Failed to open %s\n", self_executable, filename);
        exit(2);
    }
    char zero = 0;
    fwrite(data->bmpHeader, sizeof(BMP_HEADER), 1, f);
    fwrite(data->dibHeader, sizeof(DIB_HEADER), 1, f);
    fwrite(&zero, 1, data->bmpHeader->offset-(sizeof(BMP_HEADER) + sizeof(DIB_HEADER)), f);
    fwrite(data->imageData->data, sizeof(PIXEL), data->dibHeader->height*data->dibHeader->width ,f);

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
