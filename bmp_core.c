#include "bmp_core.h"

static BMP_HEADER *_read_BMP_HEADER(FILE *f);
static DIB_HEADER *_read_DIB_HEADER(FILE *f);
static IMAGE_DATA *_read_IMAGE_DATA(FILE *f, uint32_t offset, uint32_t width, uint32_t height);

static BMP_HEADER *_read_BMP_HEADER(FILE *f){
    BMP_HEADER *res = calloc(1, sizeof(BMP_HEADER));
    fseek(f, 0, SEEK_SET);

    fread(res, sizeof(BMP_HEADER), 1, f);
#ifdef DEBUG
    printf("Reading BMP header\n");
    printf("Signature: %c%c\n", res->signature[0], res->signature[1]);
    printf("Size: %d\n", res->size);
    printf("Application bytes: 0x");
    for(int i = 0; i < 4; i++){
        printf("%02X", res->application_reserved[i]);
    }
    printf("\n");
    printf("Offset: %d\n", res->offset);
    printf("\n");
#endif

    return res;
}
static DIB_HEADER *_read_DIB_HEADER(FILE *f){
    DIB_HEADER *res = calloc(1, sizeof(DIB_HEADER));
    fseek(f, sizeof(BMP_HEADER), SEEK_SET);

    fread(res, sizeof(DIB_HEADER), 1, f);
#ifdef DEBUG
    printf("Reading DIB header\n");
    printf("Header size: %d\n", res->header_size);
    printf("Image width: %d\n", res->width);
    printf("Image height: %d\n", res->height);
    printf("Color planes: %d\n", res->n_color_planes);
    printf("Bits per pixel: %d\n", res->bits_per_pixels);
    printf("Other bytes: 0x");
    for(int i = 0; i < 24; i++){
        if(i%4 == 0 && i != 0) printf(" ");
        printf("%02X", res->not_important[i]);
    }
    printf("\n\n");
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
    printf("Reading image data\n");
    printf("Pixels:\n");
    for(int i = 0; i < 5; i++){
        int n = i;
        PIXEL p = ((PIXEL *)res->data)[n];
        printf("%d:{r:%d,g:%d,b:%d,a:%d}\n", n, p.components.r, p.components.g, p.components.b, p.components.a);
    }
    printf("\n...\n");
    for(int i = 0; i < 5; i++){
        int n = width*height - i - 1;
        PIXEL p = ((PIXEL *)res->data)[n];
        printf("%d:{r:%d,g:%d,b:%d,a:%d}\n", n, p.components.r, p.components.g, p.components.b, p.components.a);
    }
    printf("\n");
#endif

    return res;
}

BMP_FILE *read_bmp_file(const char *filename){
    BMP_FILE *result = calloc(1, sizeof(BMP_FILE));
    FILE *file = fopen(filename, "rb");
    if(file == NULL) {
        fprintf(stderr, "Failed to open %s\n", filename);
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
        fprintf(stderr, "Failed to open %s\n", filename);
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
void free_bmp_structure(BMP_FILE *bmpFile){
    if(bmpFile->bmpHeader) free(bmpFile->bmpHeader);
    if(bmpFile->dibHeader) free(bmpFile->dibHeader);
    if(bmpFile->bitMasks) free(bmpFile->bitMasks);
    if(bmpFile->colorTable) free(bmpFile->colorTable);
    if(bmpFile->imageData) free(bmpFile->imageData);
    if(bmpFile->colorProfile) free(bmpFile->imageData);
    free(bmpFile);
}
