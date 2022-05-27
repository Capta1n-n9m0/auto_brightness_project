#include <stdio.h>
#include "bmp_core.h"
#include <assert.h>
#include "auto_brightness.h"
#include <unistd.h>
#include <stdbool.h>

#define EXIT_ERROR_BAD_ARGUMENT 3

char *self_filename;

int main(int argc, char **argv) {
    assert(__STDC_VERSION__ >= 202000l);
#ifdef DEBUG
    printf("Debug mode!\n");
#endif
    char *next = strtok(argv[0], "/");
    while (next != NULL) {
        self_filename = next;
        next = strtok(NULL, "/");
    }
    if(argc == 1){
        fprintf(stderr, "%s: [ FATAL ] You must provide input file\n", self_filename);
        exit(EXIT_ERROR_BAD_ARGUMENT);
    }
    bool is_output_redirected = !isatty(STDOUT_FILENO);
    bool is_input_redirected = !isatty(STDIN_FILENO);
    bool help_flag = false;
    bool output_flag = false;
    char *output_file = NULL, *input_file = NULL;
    if(is_input_redirected){
        fprintf(stderr, "%s: [ ERROR ] Input redirection is not implemented yet.\n", self_filename);
    }
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-h") == 0) help_flag = true;
        else {
            if (is_output_redirected) {
                printf("redir!\n");
                input_file = argv[i];
            } else{
                if (strcmp(argv[i], "-o") == 0) output_flag = true;
                else if(output_flag){
                    if(output_file != NULL) input_file = argv[i];
                    else output_file = argv[i];
                } else input_file = argv[i];
            }
        }
    }
    if(help_flag) printf("Help\n");
    if(input_file) printf("Input: %s\n", input_file);
    if(output_file) printf("Output: %s\n", output_file);
//    BMP_FILE *f = read_bmp_file("image.bmp");
//    BMP_FILE *copy = auto_adjust_image(f);
//
//    write_bmp_file("another_one.bmp", copy);
//
//    free_bmp_structure(copy);
//    free_bmp_structure(f);
#ifdef DEBUG
    puts("\nExiting with success!");
#endif
    return 0;
}
