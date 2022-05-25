#include <stdio.h>
#include "bmp_core.h"

int main() {
#ifdef DEBUG
    printf("Debug mode!\n");
#endif
    BMP_FILE *f = read_bmp_file("example.bmp");



    free_bmp_structure(f);
#ifdef DEBUG
    puts("\nExiting with success!");
#endif
    return 0;
}
