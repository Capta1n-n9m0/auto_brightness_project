#define _GNU_SOURCE
#include <stdio.h>
#include "bmp_core.h"
#include <assert.h>
#include "auto_brightness.h"
#include <unistd.h>
#include <stdbool.h>

#define EXIT_ERROR_BAD_ARGUMENT 3

char *self_executable = NULL;

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
    bool help_flag = false;
    bool output_flag = false;
    char *output_file = NULL, *input_file = NULL;
    if(is_input_redirected){
        fprintf(stderr, "%s: [ ERROR ] Input redirection is not implemented yet.\n", self_executable);
    }
    if(is_output_redirected){
        fprintf(stderr, "%s: [ ERROR ] Output redirection is not implemented yet.\n", self_executable);
    }
    int rez;

    while(optind < argc){
        if ((rez = getopt(argc, argv, "ho:")) != -1) {
            switch (rez) {
                case 'h':
                    help_flag = true;
                    break;
                case 'o':
                    output_flag = true;
                    output_file = optarg;
                    break;
                case '?':
                    fprintf(stderr, "%s: [WARNING] Argument error occurred: %c!\n", self_executable, rez);
                    break;
            }
        } else{
            input_file = argv[optind];
            optind++;
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
    fprintf(stderr, "\n%s: [ DEBUG ] Exiting with success!\n", self_executable);
#endif
    return 0;
}
