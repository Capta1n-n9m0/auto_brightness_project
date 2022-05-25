#include <stdio.h>
#include "bmp_core.h"
#include <assert.h>
#include "auto_brightness.h"

int main() {
    assert(__STDC_VERSION__ >= 202000l);
#ifdef DEBUG
    printf("Debug mode!\n");
#endif
    BMP_FILE *f = read_bmp_file("example.bmp");
    BMP_FILE *copy = auto_adjust_image(f);

    write_bmp_file("another_one.bmp", copy);

    free_bmp_structure(copy);
    free_bmp_structure(f);
#ifdef DEBUG
    puts("\nExiting with success!");
#endif
    return 0;
}
