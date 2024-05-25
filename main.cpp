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


int main()
{

    printf("Enter '1' to enter text to the end\n'2' to add new line\n'3' to load TO file\n'4' to load FROM file\n'5' to print the text to console\n'6' to insert text\n'7' to search for text\n");
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