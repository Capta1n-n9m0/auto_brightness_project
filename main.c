#define _GNU_SOURCE
#include <stdio.h>
#include "bmp_core.h"
#include <assert.h>
#include "auto_brightness.h"
#include <unistd.h>
#include <stdbool.h>

#define EXIT_ERROR_BAD_ARGUMENT 3

char *self_executable = NULL;

static void print_help();

int main(int argc, char **argv) {
    assert(__STDC_VERSION__ >= 202000l);
    char *self_executable_full_path = calloc(strlen(argv[0]) + 1, sizeof (char));
    strcpy(self_executable_full_path, argv[0]);
    char *next = strtok(self_executable_full_path, "/");
    while (next != NULL) {
        self_executable = next;
        next = strtok(NULL, "/");
    }
#ifdef DEBUG
    fprintf(stderr, "%s: [ DEBUG ] Debug mode!\n", self_executable);
#endif
    if(argc == 1){
        fprintf(stderr, "%s: [ FATAL ] You must provide input file\n", self_executable);
        exit(EXIT_ERROR_BAD_ARGUMENT);
    }
    bool is_output_redirected = !isatty(STDOUT_FILENO);
    bool is_input_redirected = !isatty(STDIN_FILENO);
    bool is_color_in_big_endian = true;
    bool help_flag = false;
    char *output_file = NULL, *input_file = NULL;
    if(is_input_redirected){
        fprintf(stderr, "%s: [ ERROR ] Input redirection is not implemented yet.\n", self_executable);
    }
    if(is_output_redirected){
        fprintf(stderr, "%s: [ ERROR ] Output redirection is not implemented yet.\n", self_executable);
    }
    int argument;
    const char *options = "ho:e:";
    while(optind < argc){
        if ((argument = getopt(argc, argv, options)) != -1) {
            switch (argument) {
                case 'h':
                    help_flag = true;
                    break;
                case 'o':
                    output_file = optarg;
                    break;
                case 'e':
                    if(optarg[0] == 'l') is_color_in_big_endian = false;
                    break;
                case '?':
                    fprintf(stderr, "%s: [WARNING] Argument error! Unknown argument %s at position %d!\n", self_executable, argv[optind-1], optind-1);
                    break;
            }
        } else{
            input_file = argv[optind];
            optind++;
        }
    }
    if(help_flag) print_help();
    if(input_file) printf("Input: %s\n", input_file);
    if(output_file) printf("Output: %s\n", output_file);
    BMP_FILE *f = read_bmp_file("example.bmp", false);
    BMP_FILE *copy = auto_adjust_image(f);

    write_bmp_file("result.bmp", copy, false);

    free_bmp_structure(copy);
    free_bmp_structure(f);
    fprintf(stderr, "%s: [  INFO ] If your image does not look right, or does not exist, try different endian option.\n", self_executable);
#ifdef DEBUG
    fprintf(stderr, "\n%s: [ DEBUG ] Exiting with success!\n", self_executable);
#endif
    return 0;
}

static void print_help(){
    fprintf(stderr, "\n");
    int line_len = fprintf(stderr, "======= %s HELP PAGE =======", self_executable);
    fprintf(stderr, "\n\n");
    fprintf(stderr, "Usage: %s [option] input_bmp_file...\n", self_executable);
    fprintf(stderr, "OPTIONS:\n");
    fprintf(stderr, "  -h           Displays this information.\n");
    fprintf(stderr, "  -o <file>    Would write output to the file.\n");
    fprintf(stderr, "  -e <b,l>     Specify the endianness of pixels. Defaults to big endian if not specified.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "%s will take .bmp image and adjust it's brightness and contrast.\n", self_executable);
    fprintf(stderr, "Input and output can be redirected. Then %s would read and/or write binary.\n", self_executable);
    fprintf(stderr, "\n");
    for(int i = 0; i < line_len; i++) fprintf(stderr, "=");
    fprintf(stderr, "\n\n");
}
