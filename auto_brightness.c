#include "auto_brightness.h"
#include <math.h>


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
    for(int i = 0; i < width*height; i++){
        image[i].components.r -= r_min;
        image[i].components.g -= g_min;
        image[i].components.b -= b_min;
    }
    double r_ratio, g_ratio, b_ratio;
    if(r_max != 0) r_ratio = MAXIMUM_PIXEL_COLOR_VALUE / r_max;
    else r_ratio = NAN;
    if(g_max != 0)g_ratio = MAXIMUM_PIXEL_COLOR_VALUE / g_max;
    else g_ratio = NAN;
    if(b_max != 0)b_ratio = MAXIMUM_PIXEL_COLOR_VALUE / b_max;
    else b_ratio = NAN;
    for(int i = 0; i < width*height; i++){
        if(r_ratio != NAN) image[i].components.r = (int)round(r_ratio * image[i].components.r);
        else image[i].components.r = r_min;
        if(g_ratio != NAN) image[i].components.g = (int)round(g_ratio * image[i].components.g);
        else image[i].components.g = g_min;
        if(g_ratio != NAN) image[i].components.b = (int)round(b_ratio * image[i].components.b);
        else image[i].components.b = b_min;
    }


    return res;
}
