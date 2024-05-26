#include <iostream>

void enter_text(char** current_text) {
    printf("Enter the text: ");
    char buffer[100];
    fgets(buffer, 100, stdin);
    size_t length_of_current_text = strlen(*current_text);
    size_t length_of_input = strlen(buffer);
    *current_text = (char*)realloc(*current_text, length_of_current_text + length_of_input + 1);

    strcat(*current_text, buffer);

    if ((*current_text)[strlen(*current_text) - 1] == '\n') {
        (*current_text)[strlen(*current_text) - 1] = '\0';
    }
}

void add_new_line(char** current_text) {
    size_t length_of_current_text = strlen(*current_text);
    *current_text = (char*)realloc(*current_text, length_of_current_text + 2);
    strcat(*current_text, "\n");

    printf("New line created!\n");
}

void save_to_file(char* filename, char* current_text) {
    FILE *savefile = fopen(filename, "w");
    if (savefile == nullptr) {
        printf("Error opening file!\n");
        return;
    }
    fprintf(savefile, "%s", current_text);
    fclose(savefile);

    printf("Saved to file!\n");
}

void load_from_file(char* filename, char** current_text) {

    FILE *loadfile = fopen(filename, "r");
    if (loadfile == nullptr) {
        printf("Error opening file!\n");
        return;
    }

    free(*current_text);
    *current_text = (char*)malloc(sizeof(char));
    (*current_text)[0] = '\0';

    char buffer[100];
    while (fgets(buffer, 100, loadfile) != nullptr) {
        size_t length_of_current_text = strlen(*current_text);
        size_t length_of_input = strlen(buffer);
        *current_text = (char*)realloc(*current_text, length_of_current_text + length_of_input + 1);
        strcat(*current_text, buffer);
    }
    fclose(loadfile);

    printf("Loaded from file!\n");
}

void insert_text(char** current_text) {
    printf("Enter the line and position to insert the text: ");
    int line, position;
    scanf("%d %d", &line, &position);

    printf("Enter the text to insert: ");
    char buffer[100];
    while (getchar() != '\n'){}
    fgets(buffer, 100, stdin);

    size_t buffer_len = strlen(buffer);
    if (buffer[buffer_len - 1] == '\n') {
        buffer[buffer_len - 1] = '\0';
        buffer_len--;
    }

    int current_line = 0;
    int current_position = 0;
    char* text = *current_text;
    size_t text_len = strlen(text);

    while(current_line <= line && current_position <= text_len) {
       if(text[current_position] == '\n') {
           current_line++;
           current_position++;
       }
        current_position++;

        if (current_line == line) {
            break;
        }

    }

    size_t insertion_position = current_position + position - 1;
    size_t new_text_len = text_len + buffer_len;
    *current_text = (char*)realloc(*current_text, new_text_len + 1);

    text = *current_text;

    memmove(text + insertion_position + buffer_len, text + insertion_position, text_len - insertion_position + 1);
    memcpy(text + insertion_position, buffer, buffer_len);

    printf("Text inserted!\n");
}

int main()
{

    printf("Enter '1' to enter text to the end\n'2' to add new line\n'3' to save TO file\n'4' to load FROM file\n'5' to print the text to console\n'6' to insert text\n'7' to search for text\n '0' to exit\n\n");
    char* string = (char*)malloc(sizeof(char));
    string[0] = '\0';

    while(true) {
        printf("Enter the command: ");
        char users_choice = getchar();
        while (getchar() != '\n');
        switch (users_choice) {
            case '1': {
                enter_text(&string);
                //printf("%s\n", string);
                break;
            }
            case '2': {
                add_new_line(&string);
                //printf("%s\n", string);
                break;
            }

            case '3': {
                char filename[100];
                printf("Enter the file name for saving: ");
                fgets(filename, 100, stdin);
                filename[strcspn(filename, "\n")] = '\0';
                // char *filename = "/Users/dlitvakk21/CLionProjects/text-editor/savetext.txt";
                save_to_file(filename, string);
                break;
            }

            case '4': {
                char filename[100];
                printf("Enter the file name for saving: ");
                fgets(filename, 100, stdin);
                filename[strcspn(filename, "\n")] = '\0';
                // char *filename = "/Users/dlitvakk21/CLionProjects/text-editor/loadtext.txt";
                load_from_file(filename, &string);
                //printf("%s\n", string);
                break;
            }

            case '5': {
                printf("%s\n", string);
                break;
            }

            case '6': {
                insert_text(&string);
                break;
            }

            case '0': {
                printf("Thanks for using the program!\n");
                free(string);
                exit(0);
            }

            default: printf("Invalid command! Try again.\n");
        }
    }

    return 0;
}