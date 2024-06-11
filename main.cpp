#include <iostream>
#include <stack>
using namespace std;

class TextEditor {
    char* text;
    size_t length;
    stack<char*> undoStack;
    stack<char*> redoStack;
    char* bufferCopy;


    void capacity(size_t newLength) {
        if (newLength > length) {
            text = (char*)realloc(text, newLength);
            length = newLength;
        }
    }

    void saveCommand(stack<char*>& stack) {
        char* newCommand = (char*)malloc(strlen(text) + 1);
        strcpy(newCommand, text);
        stack.push(newCommand);
    }

    void emptyRedoStack() {
        while (!redoStack.empty()) {
            free(redoStack.top());
            redoStack.pop();
        }
    }

public:
    TextEditor() : text(nullptr), length(0), bufferCopy(nullptr) { // A place for constructing the object, setting default values and memory allocation
        text = (char*)malloc(1);
        text[0] = '\0';
    }

    ~TextEditor() { // A place where memory deallocation and object descrutrion takes place
        free(text);
        while (!undoStack.empty()) {
            free(undoStack.top());
            undoStack.pop();
        }

        emptyRedoStack();
        free(bufferCopy);
    }

    char* getText() const { // const means that this method does not change the object
        return text;
    }

    void enterText(const char* newText) {
        size_t newLength = strlen(newText);
        size_t oldLength = strlen(text);
        capacity(oldLength + newLength + 1);
        strcat(text, newText);
        saveCommand(undoStack);
        emptyRedoStack();
    }

    void addNewLine() {
        size_t oldLength = strlen(text);
        capacity(oldLength + 2);
        strcat(text, "\n");
        saveCommand(undoStack);
        emptyRedoStack();
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
        saveCommand(undoStack);
        emptyRedoStack();
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

    void deleteText(size_t line, size_t posInLine, size_t numSymbols) {
        size_t current_line = 0;
        size_t current_position = 0;
        size_t text_len = strlen(text);

        while (current_line <= line && current_position <= text_len) {
            if (text[current_position] == '\n') {
                current_line++;
                current_position++;
            }
            current_position++;

            if (current_line == line) {
                break;
            }
        }

        size_t start_position = current_position + posInLine - 1;
        size_t end_position = start_position + numSymbols;

        memmove(text + start_position, text + end_position, text_len - end_position + 1);
        saveCommand(undoStack);
        emptyRedoStack();
    }

    void undo() {
        if (!undoStack.empty()) {
            saveCommand(redoStack);
            free(text);
            undoStack.pop();
            text = undoStack.top();
            cout << "Successful undo!" << endl;
        }

        else {
            cout << "No commands to undo!" << endl;
        }
    }

    void redo() {
        if (!redoStack.empty()) {
            saveCommand(undoStack);
            free(text);
            // redoStack.pop();
            text = redoStack.top();
            redoStack.pop();
            cout << "Successful redo!" << endl;
        }

        else {
            cout << "No commands to redo!" << endl;
        }
    }
    void cutText(size_t line, size_t posInLine, size_t numSymbols) {
        size_t current_line = 0;
        size_t current_position = 0;
        size_t text_len = strlen(text);

        while (current_line <= line && current_position <= text_len) {
            if (text[current_position] == '\n') {
                current_line++;
                current_position++;
            }
            current_position++;

            if (current_line == line) {
                break;
            }
        }

        size_t start_position = current_position + posInLine - 1;
        if (start_position + numSymbols > text_len) {
            numSymbols = text_len - start_position;
        }

        bufferCopy = (char*)realloc(bufferCopy, numSymbols + 1);
        strncpy(bufferCopy, text + start_position, numSymbols);
        bufferCopy[numSymbols] = '\0';

        deleteText(line, posInLine, numSymbols);
    }

    void copyText(size_t line, size_t posInLine, size_t numSymbols) {
        size_t current_line = 0;
        size_t current_position = 0;
        size_t text_len = strlen(text);

        while (current_line <= line && current_position <= text_len) {
            if (text[current_position] == '\n') {
                current_line++;
                current_position++;
            }
            current_position++;

            if (current_line == line) {
                break;
            }
        }

        size_t start_position = current_position + posInLine - 1;
        if (start_position + numSymbols > text_len) {
            numSymbols = text_len - start_position;
        }

        bufferCopy = (char*)realloc(bufferCopy, numSymbols + 1);
        strncpy(bufferCopy, text + start_position, numSymbols);
        bufferCopy[numSymbols] = '\0';
    }

    void pasteText(size_t line, size_t posInLine) {
        if (bufferCopy == nullptr) {
            cout << "Clipboard is empty!" << endl;
            return;
        }

        insert(line, posInLine, bufferCopy);
    }
};

int main() {
    TextEditor editor;
    char command;

    cout << "Enter 'a' to enter text to the end" << endl;
    cout << "'b' to add new line" << endl;
    cout << "'c' to save TO file" << endl;
    cout << "'d' to load FROM file" << endl;
    cout << "'e' to print the text to console" << endl;
    cout << "'f' to insert text" << endl;
    cout << "'g' to search for text" << endl;
    cout << "'h' to delete" << endl;
    cout << "'i' to undo command" << endl;
    cout << "'j' to redo command" << endl;
    cout << "'k' to cut text" << endl;
    cout << "'l' to copy text" << endl;
    cout << "'m' to paste text" << endl;
    cout << "'o' TO EXIT" << endl;
    cout << endl;

    do {
        cout << "Enter the command: ";
        cin >> command;

        switch (command) {
            case 'a': {
                cout << "Enter the text to add: ";
                char newText[256];
                cin.ignore();
                cin.getline(newText, sizeof(newText));
                editor.enterText(newText);
                break;
            }
            case 'b': {
                editor.addNewLine();
                cout << "New line added!" << endl;
                break;
            }
            case 'c': {
                char filename[100];
                cout << "Enter the file name for saving: ";
                cin >> filename;
                editor.saveToFile(filename);
                cout << "Text saved to file!" << endl;
                break;
            }
            case 'd': {
                char filename[100];
                cout << "Enter the file name for loading: ";
                cin >> filename;
                editor.loadFromFile(filename);
                cout << "Text loaded from file!" << endl;
                break;
            }
            case 'e': {
                editor.print();
                break;
            }
            case 'f': {
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
            case 'g': {
                cout << "Enter the text to search: ";
                char searchText[256];
                cin.ignore();
                cin.getline(searchText, sizeof(searchText));
                editor.findText(searchText);
                break;
            }
            case 'h': {
                size_t line, pos, numSymbols;
                cout << "Enter the line number: ";
                cin >> line;
                cout << "Enter the position in line: ";
                cin >> pos;
                cout << "Enter the number of symbols to delete: ";
                cin >> numSymbols;
                editor.deleteText(line, pos, numSymbols);
                cout << "Text deleted!" << endl;
                break;
            }
            case 'i': {
                editor.undo();
                break;
            }
            case 'j': {
                editor.redo();
                break;
            }
            case 'k': {
                size_t line, pos, numSymbols;
                cout << "Enter the line number: ";
                cin >> line;
                cout << "Enter the position in line: ";
                cin >> pos;
                cout << "Enter the number of symbols to cut: ";
                cin >> numSymbols;
                editor.cutText(line, pos, numSymbols);
                cout << "Text cut!" << endl;
                break;
            }
            case 'l': {
                size_t line, pos, numSymbols;
                cout << "Enter the line number: ";
                cin >> line;
                cout << "Enter the position in line: ";
                cin >> pos;
                cout << "Enter the number of symbols to copy: ";
                cin >> numSymbols;
                editor.copyText(line, pos, numSymbols);
                cout << "Text copied!" << endl;
                break;
            }

            case 'm': {
                size_t line, pos;
                cout << "Enter the line number: ";
                cin >> line;
                cout << "Enter the position in line: ";
                cin >> pos;
                editor.pasteText(line, pos);
                cout << "Text pasted!" << endl;
                break;
            }

            case 'o': {
                cout << "Thanks for using the program!" << endl;
                break;
            }
            default: {
                cout << "Invalid command! Try again." << endl;
                break;
            }
        }

    } while (command != 'o');

    return 0;
}

// /Users/dlitvakk21/CLionProjects/text-editor/savetext.txt
// /Users/dlitvakk21/CLionProjects/text-editor/loadtext.txt