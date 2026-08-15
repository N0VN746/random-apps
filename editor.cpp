#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class DocumentEditor {
private:
    std::vector<std::string> document;
    std::string currentFilename;

    void displayMenu() const {
        std::cout << "\n=== CLI DOCUMENT EDITOR ===\n";
        std::cout << "Current File: " << (currentFilename.empty() ? "[Untitled]" : currentFilename) << "\n";
        std::cout << "1. View Document\n";
        std::cout << "2. Insert Line\n";
        std::cout << "3. Delete Line\n";
        std::cout << "4. Save File\n";
        std::cout << "5. Open File\n";
        std::cout << "6. Exit\n";
        std::cout << "Choose an option: ";
    }

public:
    void viewDocument() const {
        std::cout << "\n--- Document Content ---\n";
        if (document.empty()) {
            std::cout << "[Empty Document]\n";
            return;
        }
        for (size_t i = 0; i < document.size(); ++i) {
            std::cout << i + 1 << ": " << document[i] << "\n";
        }
        std::cout << "------------------------\n";
    }

    void insertLine() {
        viewDocument();
        size_t lineNum;
        std::cout << "Enter line number to insert at (1 to " << document.size() + 1 << "): ";
        std::cin >> lineNum;
        std::cin.ignore(); // Clear newline buffer

        if (lineNum < 1 || lineNum > document.size() + 1) {
            std::cout << "Invalid line number.\n";
            return;
        }

        std::string text;
        std::cout << "Enter text: ";
        std::getline(std::cin, text);

        document.insert(document.begin() + (lineNum - 1), text);
        std::cout << "Line inserted.\n";
    }

    void deleteLine() {
        viewDocument();
        if (document.empty()) return;

        size_t lineNum;
        std::cout << "Enter line number to delete (1 to " << document.size() << "): ";
        std::cin >> lineNum;
        std::cin.ignore();

        if (lineNum < 1 || lineNum > document.size()) {
            std::cout << "Invalid line number.\n";
            return;
        }

        document.erase(document.begin() + (lineNum - 1));
        std::cout << "Line deleted.\n";
    }

    void saveFile() {
        if (currentFilename.empty()) {
            std::cout << "Enter filename to save as: ";
            std::cin >> currentFilename;
            std::cin.ignore();
        }

        std::ofstream outFile(currentFilename);
        if (!outFile) {
            std::cout << "Error saving file.\n";
            return;
        }

        for (const auto& line : document) {
            outFile << line << "\n";
        }
        std::cout << "File saved successfully as " << currentFilename << "\n";
    }

    void openFile() {
        std::cout << "Enter filename to open: ";
        std::cin >> currentFilename;
        std::cin.ignore();

        std::ifstream inFile(currentFilename);
        if (!inFile) {
            std::cout << "Error: Could not open file.\n";
            currentFilename = "";
            return;
        }

        document.clear();
        std::string line;
        while (std::getline(inFile, line)) {
            document.push_back(line);
        }
        std::cout << "File loaded successfully. Total lines: " << document.size() << "\n";
    }

    void run() {
        int choice = 0;
        while (choice != 6) {
            displayMenu();
            if (!(std::cin >> choice)) {
                std::cout << "Please enter a valid number.\n";
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                continue;
            }
            std::cin.ignore();

            switch (choice) {
                case 1: viewDocument(); break;
                case 2: insertLine(); break;
                case 3: deleteLine(); break;
                case 4: saveFile(); break;
                case 5: openFile(); break;
                case 6: std::cout << "Exiting editor.\n"; break;
                default: std::cout << "Invalid option.\n"; break;
            }
        }
    }
};

int main() {
    DocumentEditor editor;
    editor.run();
    return 0;
}
