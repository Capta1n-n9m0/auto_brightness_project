#include "bmp_core.h"

BMP_HEADER *_read_BMP_HEADER(FILE *f);
DIB_HEADER *_read_DIB_HEADER(FILE *f);

typedef uint8_t BYTE;

BYTE read_byte(FILE *f){
    BYTE res;
    fread(&res, sizeof(BYTE), 1, f);
    return res;
}

BMP_HEADER *_read_BMP_HEADER(FILE *f){
    BMP_HEADER *res = calloc(1, sizeof(BMP_HEADER));
    fseek(f, 0, SEEK_SET);

    fread(res->signature, sizeof(uint8_t), 2, f);
    fread(&res->size, sizeof(uint32_t), 1, f);
    fread(res->application_reserved, sizeof(uint8_t), 4, f);
    fread(&res->offset, sizeof(uint32_t), 1, f);
#ifdef DEBUG
    printf("Signature: %c%c\n", res->signature[0], res->signature[1]);
    printf("Size: %d\n", res->size);
    printf("Application bytes: 0x");
    for(int i = 0; i < 4; i++){
        printf("%02X", res->application_reserved[i]);
    }
    printf("\n");
    printf("Offset: %d\n", res->offset);
#endif
    return res;
}

DIB_HEADER *_read_DIB_HEADER(FILE *f){

}

BMP_FILE *read_bmp_file(const char *filename){
    BMP_FILE *result = calloc(1, sizeof(BMP_FILE));
    FILE *file = fopen(filename, "rb");
    if(file == NULL) {
        fprintf(stderr, "Failed to open %s\n", filename);
        exit(2);
    }
    result->bmpHeader = _read_BMP_HEADER(file);
    //424D48C0 12000000 00004600



    fclose(file);
    return result;
}
int write_bmp_file(const char *filename, BMP_FILE *data){

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
