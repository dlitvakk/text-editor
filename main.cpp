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
                printf("%s\n", string);
                break;
            }
            case '2': {
                add_new_line(&string);
                printf("New line created!\n");
                printf("%s\n", string);
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
                printf("%s\n", string);
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