#include <iostream>
#include <stack>
#include <cstring>
#include <dlfcn.h>


using namespace std;

class TextEditor {
    char* text;
    size_t length;
    stack<char*> undo;
    stack<char*> redo;
    char* bufferCopy;
    int cursorLine;
    int cursorPosition;

    void capacity(size_t newSize) {
        text = (char*)realloc(text, newSize);
    }

    void saveCommand(stack<char*>& commandStack) {
        char* temp = (char*)malloc(strlen(text) + 1);
        strcpy(temp, text);
        commandStack.push(temp);
    }

    void emptyRedoStack() {
        while (!redo.empty()) {
            free(redo.top());
            redo.pop();
        }
    }

    int calculateCursorIndex(int line, int pos) const {
        int index = 0;
        int currentLine = 0;
        int currentPosition = 0;
        while (text[index] != '\0') {
            if (currentLine == line && currentPosition == pos) {
                break;
            }
            if (text[index] == '\n') {
                currentLine++;
                currentPosition = 0;
            } else {
                currentPosition++;
            }
            index++;
        }
        return index;
    }

public:
    TextEditor() : text(nullptr), length(0), bufferCopy(nullptr), cursorLine(0), cursorPosition(0) {
        text = (char*)malloc(1);
        text[0] = '\0';
    }

    ~TextEditor() {
        free(text);
        while (!undo.empty()) {
            free(undo.top());
            undo.pop();
        }
        emptyRedoStack();
        if (bufferCopy != nullptr) {
            free(bufferCopy);
        }
    }

    void setCursorPosition(int line, int pos) {
        cursorLine = line;
        cursorPosition = pos;
    }

    void enterText(const char* newText) {
        int cursorIndex = calculateCursorIndex(cursorLine, cursorPosition);
        size_t newLength = strlen(newText);
        size_t oldLength = strlen(text);
        capacity(oldLength + newLength + 1);

        memmove(text + cursorIndex + newLength, text + cursorIndex, oldLength - cursorIndex + 1);
        memcpy(text + cursorIndex, newText, newLength);

        saveCommand(undo);
        emptyRedoStack();
    }

    void addNewLine() {
        enterText("\n");
    }

    void insertText(const char* newText) {
        enterText(newText);
    }

    void insertWithReplacement(const char* newText, int numSymbols) {
        deleteText(numSymbols);
        insertText(newText);
    }

    void deleteText(int numSymbols) {
        int cursorIndex = calculateCursorIndex(cursorLine, cursorPosition);
        int text_len = strlen(text);
        if (cursorIndex + numSymbols > text_len) {
            numSymbols = text_len - cursorIndex;
        }

        memmove(text + cursorIndex, text + cursorIndex + numSymbols, text_len - cursorIndex - numSymbols + 1);
        saveCommand(undo);
        emptyRedoStack();
    }

    void cutText(int numSymbols) {
        copyText(numSymbols);
        deleteText(numSymbols);
    }

    void copyText(int numSymbols) {
        int cursorIndex = calculateCursorIndex(cursorLine, cursorPosition);
        int text_len = strlen(text);
        if (cursorIndex + numSymbols > text_len) {
            numSymbols = text_len - cursorIndex;
        }

        bufferCopy = (char*)realloc(bufferCopy, numSymbols + 1);
        strncpy(bufferCopy, text + cursorIndex, numSymbols);
        bufferCopy[numSymbols] = '\0';
    }

    void pasteText() {
        if (bufferCopy == nullptr) {
            cout << "Clipboard is empty!" << endl;
            return;
        }

        insertText(bufferCopy);
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
            int lineNumber = 0;
            int linePosition = 0;
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

    void undoCommand() {
        if (!undo.empty()) {
            saveCommand(redo);
            free(text);
            text = undo.top();
            undo.pop();
            cout << "Successful undo!" << endl;
        } else {
            cout << "No commands to undo!" << endl;
        }
    }

    void redoCommand() {
        if (!redo.empty()) {
            saveCommand(undo);
            free(text);
            text = redo.top();
            redo.pop();
            cout << "Successful redo!" << endl;
        } else {
            cout << "No commands to redo!" << endl;
        }
    }
};

class CaesarCipher {
    void* handle;
    typedef char* (*EncryptFunc)(char*, int);
    typedef char* (*DecryptFunc)(char*, int);
    EncryptFunc encryptFunc;
    DecryptFunc decryptFunc;

public:
    CaesarCipher(const char* libPath);
    ~CaesarCipher();
    char* encryptText(char* text, int key);
    char* decryptText(char* text, int key);
};

CaesarCipher::CaesarCipher(const char* libPath) {
    handle = dlopen(libPath, RTLD_LAZY);
    if (!handle) {
        cerr << "Failed to load library: " << dlerror() << std::endl;
        exit(1);
    }

    encryptFunc = (EncryptFunc)dlsym(handle, "encrypt");
    if (!encryptFunc) {
        std::cerr << "Failed to load encrypt function: " << dlerror() << std::endl;
        dlclose(handle);
        exit(1);
    }
    decryptFunc = (DecryptFunc)dlsym(handle, "decrypt");
    if (!decryptFunc) {
        std::cerr << "Failed to load decrypt function: " << dlerror() << std::endl;
        dlclose(handle);
        exit(1);
    }
}
CaesarCipher::~CaesarCipher() {
    dlclose(handle);
}

char* CaesarCipher::encryptText(char* text, int key) {
    return encryptFunc(text, key);
}

char* CaesarCipher::decryptText(char* text, int key) {
    return decryptFunc(text, key);
}
int main() {
    TextEditor editor;
    char command;
    const char* libPath ="/Users/dlitvakk21/CLionProjects/untitled3/ceaser.so";
    CaesarCipher cipher(libPath);

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
    cout << "'n' to set cursor position" << endl;
    cout << "'0' TO EXIT" << endl;
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
                cout << "Enter the text to insert: ";
                char newText[256];
                cin.ignore();
                cin.getline(newText, sizeof(newText));
                editor.insertText(newText);
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
                int numSymbols;
                cout << "Enter the number of symbols to delete: ";
                cin >> numSymbols;
                editor.deleteText(numSymbols);
                cout << "Text deleted!" << endl;
                break;
            }
            case 'i': {
                editor.undoCommand();
                break;
            }
            case 'j': {
                editor.redoCommand();
                break;
            }
            case 'k': {
                int numSymbols;
                cout << "Enter the number of symbols to cut: ";
                cin >> numSymbols;
                editor.cutText(numSymbols);
                cout << "Text cut!" << endl;
                break;
            }
            case 'l': {
                int numSymbols;
                cout << "Enter the number of symbols to copy: ";
                cin >> numSymbols;
                editor.copyText(numSymbols);
                cout << "Text copied!" << endl;
                break;
            }
            case 'm': {
                editor.pasteText();
                cout << "Text pasted!" << endl;
                break;
            }
            case 'n': {
                int line, column;
                cout << "Enter the line number: ";
                cin >> line;
                cout << "Enter the column number: ";
                cin >> column;
                editor.setCursorPosition(line, column);
                cout << "Cursor position set to line " << line << ", column " << column << "!" << endl;
                break;
            }
            case 'o': {
                cout << "Enter the text to encrypt: ";
                char text[256];
                cin.ignore();
                cin.getline(text, sizeof(text));
                int key;
                cout << "Enter the key: ";
                cin >> key;

                char* encryptedText = cipher.encryptText(text, key);
                cout << "Encrypted text: " << encryptedText << endl;
                delete[] encryptedText; // Remember to free allocated memory
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
