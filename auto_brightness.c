#include "auto_brightness.h"
#include <stdlib.h>
#include <string.h>

BMP_FILE *auto_adjust_image(BMP_FILE *input){
    BMP_FILE *res = copy_bmp_structure(input);

    PIXEL *image = (PIXEL *) res->imageData->data;
    uint32_t width = input->dibHeader->width, height = input->dibHeader->height;
    uint8_t r_min = 255, r_max = 0;
    uint8_t g_min = 255, g_max = 0;
    uint8_t b_min = 255, b_max = 0;
    for(int i = 0; i < width*height; i++){
        if(image[i].components.r < r_min) r_min = image[i].components.r;
        if(image[i].components.g < g_min) g_min = image[i].components.g;
        if(image[i].components.b < b_min) b_min = image[i].components.b;
    }
    for(int i = 0; i < width*height; i++){
        if(image[i].components.r > r_max) r_max = image[i].components.r;
        if(image[i].components.g > g_max) g_max = image[i].components.g;
        if(image[i].components.b > b_max) b_max = image[i].components.b;
    }
    r_max -= r_min;
    g_max -= g_min;
    b_max -= b_min;
    printf("%d, %d, %d\n", r_min, g_min, b_min);
    for(int i = 0; i < width*height; i++){
        image[i].components.r -= r_min;
        image[i].components.g -= g_min;
        image[i].components.b -= b_min;
    }
    double r_ratio, g_ratio, b_ratio;
    if(r_min != 0) r_ratio = r_max / r_min;
    else r_ratio = r_max;
    if(g_min != 0)g_ratio = g_max / g_min;
    else g_ratio = g_max;
    if(b_min != 0)b_ratio = b_max / b_min;
    else b_ratio = b_max;
    for(int i = 0; i < width*height; i++){
        image[i].components.r *= r_ratio;
        image[i].components.g *= g_ratio;
        image[i].components.b *= b_ratio;
    }


    return res;
}
