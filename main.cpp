#include <iostream>
using namespace std;

class TextEditor {
    char* text;
    size_t length;

    void capacity(size_t newLength) {
        if (newLength > length) {
            text = (char*)realloc(text, newLength);
            length = newLength;
        }
    }

public:
    TextEditor() : text(nullptr), length(0) { // A place for constructing the object, setting default values and memory allocation
        text = (char*)malloc(1);
        text[0] = '\0';
    }

    ~TextEditor() { // A place where memory deallocation and object descrutrion takes place
        free(text);
    }

    char* getText() const { // const means that this method does not change the object
        return text;
    }

    void enterText(const char* newText) {
        size_t newLength = strlen(newText);
        size_t oldLength = strlen(text);
        capacity(oldLength + newLength + 1);
        strcat(text, newText);
    }

    void addNewLine() {
        size_t oldLength = strlen(text);
        capacity(oldLength + 2);
        strcat(text, "\n");
    }

    void insert(size_t line, size_t posInLine, char* newText) {
        size_t buffer_len = strlen(newText);
        if (newText[buffer_len - 1] == '\n') {
            newText[buffer_len - 1] = '\0';
            buffer_len--;
        }

        int current_line = 0;
        int current_position = 0;
        size_t text_len = strlen(text);

        while (current_line <= line && current_position <= static_cast<int>(text_len)) {
            if (text[current_position] == '\n') {
                current_line++;
                current_position++;
            }
            current_position++;

            if (current_line == line) {
                break;
            }
        }

        size_t insertion_position = current_position + posInLine - 1;
        size_t new_text_len = text_len + buffer_len;
        text = (char*)realloc(text, new_text_len + 1);

        memmove(text + insertion_position + buffer_len, text + insertion_position, text_len - insertion_position + 1);
        memcpy(text + insertion_position, newText, buffer_len);
    }

    void clear() {
        capacity(1);
        text[0] = '\0';
    }

    void loadFromFile(const char* filename) {
        FILE* file = fopen(filename, "r");
        if (!file) {
            cerr << "Error opening file!" << endl;
            return;
        }
        clear();
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), file)) {
            enterText(buffer);
        }
        fclose(file);
    }

    void saveToFile(const char* filename) {
        FILE* file = fopen(filename, "w");
        if (!file) {
            cerr << "Error opening file!" << endl;
            return;
        }
        fputs(text, file);
        fclose(file);
    }

    void print() const {
        cout << text << endl;
    }

    void findText(const char* searchText) const {
        const char* foundPosition = text;
        bool found = false;

        while ((foundPosition = strstr(foundPosition, searchText)) != nullptr) {

            size_t lineNumber = 0;
            size_t linePosition = 0;
            for (const char* pointer = text; pointer < foundPosition; ++pointer) {
                if (*pointer == '\n') {
                    lineNumber++;
                    linePosition = 1;
                } else {
                    linePosition++;
                }
            }

            cout << "Found '" << searchText << "' at line " << lineNumber << ", position " << linePosition << endl;
            foundPosition++;
            found = true;
        }

        if (!found) {
            cout << "Text is not found!" << endl;
        }
    }
};

int main() {
    TextEditor editor;
    char command;

    cout << "Enter '1' to enter text to the end" << endl;
    cout << "'2' to add new line" << endl;
    cout << "'3' to save TO file" << endl;
    cout << "'4' to load FROM file" << endl;
    cout << "'5' to print the text to console" << endl;
    cout << "'6' to insert text" << endl;
    cout << "'7' to search for text" << endl;
    cout << "'0' TO EXIT" << endl;
    cout << endl;

    do {
        cout << "Enter the command: ";
        cin >> command;

        switch (command) {
            case '1': {
                cout << "Enter the text to add: ";
                char newText[256];
                cin.ignore();
                cin.getline(newText, sizeof(newText));
                editor.enterText(newText);
                break;
            }
            case '2': {
                editor.addNewLine();
                cout << "New line added!" << endl;
                break;
            }
            case '3': {
                char filename[100];
                cout << "Enter the file name for saving: ";
                cin >> filename;
                editor.saveToFile(filename);
                cout << "Text saved to file!" << endl;
                break;
            }
            case '4': {
                char filename[100];
                cout << "Enter the file name for loading: ";
                cin >> filename;
                editor.loadFromFile(filename);
                cout << "Text loaded from file!" << endl;
                break;
            }
            case '5': {
                editor.print();
                break;
            }
            case '6': {
                size_t line, pos;
                char newText[256];
                cout << "Enter the line number: ";
                cin >> line;
                cout << "Enter the position in line: ";
                cin >> pos;
                cout << "Enter the text to insert: ";
                cin.ignore();
                cin.getline(newText, sizeof(newText));
                editor.insert(line, pos, newText);
                cout << "Text inserted!" << endl;
                break;
            }
            case '7': {
                cout << "Enter the text to search: ";
                char searchText[256];
                cin.ignore();
                cin.getline(searchText, sizeof(searchText));
                editor.findText(searchText);
                break;
            }
            case '0': {
                cout << "Thanks for using the program!" << endl;
                break;
            }
            default: {
                cout << "Invalid command! Try again." << endl;
                break;
            }
        }

    } while (command != '0');

    return 0;
}

// /Users/dlitvakk21/CLionProjects/text-editor/savetext.txt
// /Users/dlitvakk21/CLionProjects/text-editor/loadtext.txt