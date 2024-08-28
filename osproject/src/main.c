#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fields.h"  // libfdr dosya okuma için

#define MAX_SIZE 1000

void handle_yaz(char *buffer, int *buffer_length, int *cursor_position, IS input);
void handle_sil(char *buffer, int *buffer_length, int *cursor_position, IS input);
void handle_sonagit(FILE *output, char *buffer, int *buffer_length, int *cursor_position);
void handle_dur(FILE *output, char *buffer, int *buffer_length, int *cursor_position);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(1);
    }

    IS is = new_inputstruct(argv[1]);
    if (is == NULL) {
        perror(argv[1]);
        exit(1);
    }

    FILE *output = fopen("dat/cikis.dat", "w");
    if (output == NULL) {
        perror("cikis.dat");
        exit(1);
    }

    char buffer[MAX_SIZE] = {0};
    int buffer_length = 0;
    int cursor_position = 0;  // Track the cursor position

    while (get_line(is) >= 0) {
        if (strcmp(is->fields[0], "yaz:") == 0) {
            handle_yaz(buffer, &buffer_length, &cursor_position, is);
        } else if (strcmp(is->fields[0], "sil:") == 0) {
            handle_sil(buffer, &buffer_length, &cursor_position, is);
        } else if (strcmp(is->fields[0], "sonagit:") == 0) {
            handle_sonagit(output, buffer, &buffer_length, &cursor_position);
        } else if (strcmp(is->fields[0], "dur:") == 0) {
            handle_dur(output, buffer, &buffer_length, &cursor_position);
            break;
        }
    }

    jettison_inputstruct(is);
    fclose(output);
    return 0;
}

void handle_yaz(char *buffer, int *buffer_length, int *cursor_position, IS input) {
    for (int i = 1; i < input->NF; i += 2) {
        int count = atoi(input->fields[i]);
        char character = input->fields[i + 1][0];
        if (character == '\\') {
            if (strcmp(input->fields[i + 1], "\\b") == 0) {
                character = ' ';
            } else if (strcmp(input->fields[i + 1], "\\n") == 0) {
                character = '\n';
            }
        }
        for (int j = 0; j < count; j++) {
            if (*buffer_length < MAX_SIZE - 1) {
                if (*cursor_position < *buffer_length) {
                    memmove(&buffer[*cursor_position + 1], &buffer[*cursor_position], *buffer_length - *cursor_position);
                }
                buffer[(*cursor_position)++] = character;
                (*buffer_length)++;
            }
        }
    }
    buffer[*buffer_length] = '\0';  // Null-terminate the buffer
}

void handle_sil(char *buffer, int *buffer_length, int *cursor_position, IS input) {
    for (int i = 1; i < input->NF; i += 2) {
        int count = atoi(input->fields[i]);
        char character = input->fields[i + 1][0];
        while (*cursor_position > 0 && count > 0) {
            if (buffer[*cursor_position - 1] == character) {
                memmove(&buffer[*cursor_position - 1], &buffer[*cursor_position], *buffer_length - *cursor_position);
                (*buffer_length)--;
                count--;
            }
            (*cursor_position)--;
        }
    }
    buffer[*buffer_length] = '\0';  // Null-terminate the buffer
}

void handle_sonagit(FILE *output, char *buffer, int *buffer_length, int *cursor_position) {
    fwrite(buffer, 1, *cursor_position, output);
    fputs("\n", output);
    memmove(buffer, buffer + *cursor_position, *buffer_length - *cursor_position);
    *buffer_length -= *cursor_position;
    *cursor_position = 0;
}

void handle_dur(FILE *output, char *buffer, int *buffer_length, int *cursor_position) {
    fwrite(buffer, 1, *cursor_position, output);
    fputs("\n", output);
    *buffer_length = 0;
    *cursor_position = 0;
    buffer[0] = '\0';  // Reset the buffer
}
