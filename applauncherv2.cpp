#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <windows.h>

namespace fs = std::filesystem;

struct TargetApp {
    std::string name;
    std::string path;
};

// Recursive function to search for all .exe files inside a directory
void scanDirectory(const fs::path& targetDir, std::vector<TargetApp>& exeList) {
    // std::filesystem::directory_options::skip_permission_denied avoids crashes on protected system files
    auto options = fs::directory_options::skip_permission_denied;
    
    try {
        if (!fs::exists(targetDir)) return;

        for (const auto& entry : fs::recursive_directory_iterator(targetDir, options)) {
            try {
                if (entry.is_regular_file() && entry.path().extension() == ".exe") {
                    TargetApp app;
                    app.name = entry.path().filename().string();
                    app.path = entry.path().string();
                    exeList.push_back(app);
                }
            } catch (const fs::filesystem_error&) {
                // Silently skip locked or protected files
                continue;
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Directory access error: " << e.what() << "\n";
    }
}

void launchApp(const std::string& path) {
    std::cout << "\nStarting: " << path << "\n";

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcessA(
        path.c_str(), // Path to executable
        NULL,         // Command line arguments
        NULL,         // Process handles
        NULL,         // Thread handles
        FALSE,        // Inheritance options
        0,            // Creation flags
        NULL,         // Environment block
        NULL,         // Starting directory 
        &si,          
        &pi           
    )) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        std::cerr << "Windows failed to execute this file. Error Code: " << GetLastError() << "\n";
    }
}

int main() {
    std::vector<TargetApp> exeList;

    std::cout << "Scanning standard application hubs for ALL executables...\n";
    std::cout << "(This may take a moment depending on your storage speed)\n\n";

    // Targets where user applications live. Change to "C:\\" to scrape the entire drive.
    std::vector<std::string> searchPaths = {
		"C:\\",
        // Dynamically get the Local AppData folder (where apps like Discord and Chrome install)
        std::string(std::getenv("USERPROFILE")) + "\\AppData\\Local"
    };

    for (const auto& path : searchPaths) {
        scanDirectory(path, exeList);
    }

    if (exeList.empty()) {
        std::cout << "No executable files found.\n";
        return 0;
    }

    // Display the massive menu
    for (size_t i = 0; i < exeList.size(); ++i) {
        std::cout << i + 1 << ". " << exeList[i].name << " [" << exeList[i].path << "]\n";
    }

    std::cout << "\nFound " << exeList.size() << " executables.\n";
    std::cout << "Choose an application to launch (1-" << exeList.size() << "): ";
    
    std::string input;
    std::getline(std::cin, input);

    try {
        int choice = std::stoi(input);
        if (choice >= 1 && choice <= static_cast<int>(exeList.size())) {
            launchApp(exeList[choice - 1].path);
        } else {
            std::cout << "Invalid selection.\n";
        }
    } catch (const std::invalid_argument&) {
        std::cout << "Numerical input required.\n";
    }

    return 0;
}
