#include <stdio.h>
#include "bmp_core.h"
#include <assert.h>

int main() {
    assert(__STDC_VERSION__ >= 202000l);
#ifdef DEBUG
    printf("Debug mode!\n");
#endif
    BMP_FILE *f = read_bmp_file("example.bmp");

    write_bmp_file("another_one.bmp", f);

    free_bmp_structure(f);
#ifdef DEBUG
    puts("\nExiting with success!");
#endif
    return 0;
}
