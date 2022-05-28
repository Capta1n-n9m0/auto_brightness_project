#define _GNU_SOURCE
#include <stdio.h>
#include "bmp_core.h"
#include <assert.h>
#include "auto_brightness.h"
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define EXIT_ERROR_BAD_ARGUMENT 3
#define EXIT_ERROR_NO_INPUT_FILE 4
#define EXIT_ERROR_FAILED_FILE_OPERATION 5
#define EXIT_ERROR_BAD_INPUT_FILE 6

char *self_executable = NULL;

static void print_help();
static char *get_random_string(int len);


int main(int argc, char **argv) {
    assert(__STDC_VERSION__ >= 202000l);
    srand(time(NULL));
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
    char *output_filename = NULL, *input_filename = NULL;
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
                    output_filename = optarg;
                    break;
                case 'e':
                    if(optarg[0] == 'l') is_color_in_big_endian = false;
                    break;
                case '?':
                    fprintf(stderr, "%s: [WARNING] Argument error! Unknown argument %s at position %d!\n", self_executable, argv[optind-1], optind-1);
                    break;
            }
        } else{
            input_filename = argv[optind];
            optind++;
        }
    }
    if(help_flag) print_help();

    if(input_filename == NULL){
        fprintf(stderr, "%s: [ FATAL ] No input file. Please, provide input file\n", self_executable);
        exit(EXIT_ERROR_NO_INPUT_FILE);
    }

    FILE *input_file = fopen(input_filename, "rb");
    if(input_file == NULL){
        fprintf(stderr, "%s: [ FATAL ] Failed to open input file \"%s\", please check the filename.\n", self_executable, input_filename);
        fclose(input_file);
        exit(EXIT_ERROR_FAILED_FILE_OPERATION);
    }

    BMP_FILE *input_bmp = read_bmp_file(input_file, is_color_in_big_endian);
    if(input_bmp == NULL){
        fprintf(stderr, "%s: [ FATAL ] Failed reading %s as .bmp file. Please, check your file.\n", self_executable, input_filename);
        free_bmp_structure(input_bmp);
        fclose(input_file);
        exit(EXIT_ERROR_BAD_INPUT_FILE);
    }
    fclose(input_file);
    input_bmp->filename = input_filename;

    BMP_FILE *output_bmp = auto_adjust_image(input_bmp);

    FILE *output_file = NULL;
    if(output_filename) {
        output_file = fopen(output_filename, "wb");
    } else{
        output_filename = calloc(strlen(input_filename) + 1 + 1 + 8 + 4, sizeof(char));
        char *random = get_random_string(8);
        sprintf(output_filename, "%s.%s.bmp", input_filename, random);
        free(random);
        fprintf(stderr, "%s: [WARNING] No output file was provided, so defaulting to \"%s\"\n", self_executable, output_filename);
        output_file = fopen(output_filename, "wb");
    }

    output_bmp->filename = calloc(1, strlen(output_filename) + 1);
    strcpy(output_bmp->filename, output_filename);

    if(output_file == NULL){
        fprintf(stderr, "%s: [ FATAL ] Failed to open output file \"%s\", please check the filename.\n", self_executable, input_filename);
        exit(EXIT_ERROR_FAILED_FILE_OPERATION);
    }

    write_bmp_file(output_file, output_bmp, is_color_in_big_endian);

    free_bmp_structure(output_bmp);
    free_bmp_structure(input_bmp);
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
static char *get_random_string(int len) {
    char *res = calloc(sizeof(char), len+1);
    for(int i = 0; i < len; i++){
        res[i] = rand() % 26 + 'a';
    }
    return res;
}