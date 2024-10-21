#define _DEFAULT_SOURCE

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "generator.h"
#include "lexer.h"
#include "lib.h"
#include "parser.h"
#include "slice.h"
#include "sprout.h"

void file_read_to_cstr(char const *filename, char **buffer, size_t *size)
{
    FILE *fp = NULL;
    unwrap_en((fp = fopen(filename, "r")) == NULL, "Could not fopen");
    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    *buffer = calloc(1, *size + 1);
    unwrap_en(fread(*buffer, 1, *size, fp) != *size, "Could not fwrite");
    fclose(fp);
}

int main(int argc, char *argv[])
{
    struct {
        char const *input_filename;
        char *output_filename;
    } cli_options = { .output_filename = "out" };

    while (1) {
        static struct option options[] = {
            { "output", required_argument, 0,  'o' },
            {     NULL,                 0, 0, '\0' },
        };

        int option_i = 0;
        int c = getopt_long(argc, argv, "o:", options, &option_i);
        if (c == -1) {
            break;
        }

        switch (c) {
            case 'o':
                cli_options.output_filename = optarg;
                break;
            case '?':
                exit(1);
            default:
                exit(1);
        }
    }
    if (optind < argc) {
        cli_options.input_filename = argv[optind];
    }

    expect(cli_options.input_filename != NULL, "Usage: ./astrac <input-filename>");

    char *buffer;
    size_t bufferlen;
    file_read_to_cstr(cli_options.input_filename, &buffer, &bufferlen);

    DArrayToken tokens = lexer_analyse(slice_cstr(buffer));

    NodeExit root;
    expect(parse(tokens, &root), "Could not parse tokens");

    Sprout assembly = generator_generate(&root);

    printf("Generating assembly...\n\n%s\n", assembly.data);

    // Write assembly to file
    FILE *fp = fopen("out.asm", "w");
    unwrap_en(fp == NULL, "Could not open file");
    unwrap_en(fwrite(assembly.data, 1, assembly.length, fp) != assembly.length, "fwrite");
    fclose(fp);

    // Assemble and link and clean
    system("nasm -felf64 -o out.o out.asm");
    char command_buffer[512] = { 0 };
    snprintf(command_buffer, 512, "ld -o %s out.o", cli_options.output_filename);
    system(command_buffer);
    system("rm out.asm out.o");

    sprout_pluck(&assembly);
    da_free(&tokens);
    free(buffer);

    return 0;
}
