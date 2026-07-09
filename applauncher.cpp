#include <iostream>
#include <vector>
#include <string>
#include <windows.h>

// Structure to pair the application's clean name with its executable path
struct InstalledApp {
    std::string name;
    std::string exePath;
};

// Function to fetch registered apps directly from the Windows Registry
std::vector<InstalledApp> getRegistryApps() {
    std::vector<InstalledApp> foundApps;
    HKEY hKey;
    
    // Path to the registry subkey where Windows maps applications to their executables
    const char* subkey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths";

    // Open the registry key for reading
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, subkey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD index = 0;
        char nameBuffer[256];
        DWORD nameSize = sizeof(nameBuffer);

        // Enumerate through all the subkeys (e.g., "chrome.exe", "winword.exe")
        while (RegEnumKeyExA(hKey, index, nameBuffer, &nameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
            std::string appName = nameBuffer;
            HKEY hSubKey;

            // Open each specific application's subkey to read its default path value
            if (RegOpenKeyExA(hKey, appName.c_str(), 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
                char pathBuffer[MAX_PATH];
                DWORD pathSize = sizeof(pathBuffer);
                DWORD type = REG_SZ;

                // Query the (Default) value which holds the absolute file path to the .exe
                if (RegQueryValueExA(hSubKey, NULL, NULL, &type, (LPBYTE)pathBuffer, &pathSize) == ERROR_SUCCESS) {
                    InstalledApp app;
                    
                    // Format a readable name by stripping out ".exe" if present
                    size_t dotPos = appName.find(".exe");
                    app.name = (dotPos != std::string::npos) ? appName.substr(0, dotPos) : appName;
                    app.exePath = pathBuffer;

                    // Clean up potential outer quotes sometimes left in registry strings
                    if (!app.exePath.empty() && app.exePath.front() == '"' && app.exePath.back() == '"') {
                        app.exePath = app.exePath.substr(1, app.exePath.length() - 2);
                    }

                    if (!app.exePath.empty()) {
                        foundApps.push_back(app);
                    }
                }
                RegCloseKey(hSubKey);
            }
            index++;
            nameSize = sizeof(nameBuffer); // Reset buffer size for the next loop
        }
        RegCloseKey(hKey);
    }
    return foundApps;
}

// Function to launch the executable safely using the Windows API
void launchApp(const std::string& path) {
    std::cout << "Starting: " << path << "\n";

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // CreateProcessA handles paths containing spaces much cleaner than system()
    if (CreateProcessA(
        path.c_str(), // Absolute path to executable
        NULL,         // Command line arguments
        NULL,         // Process handle not inheritable
        NULL,         // Thread handle not inheritable
        FALSE,        // Set handle inheritance to FALSE
        0,            // No creation flags
        NULL,         // Use parent's environment block
        NULL,         // Use parent's starting directory 
        &si,          // Pointer to STARTUPINFO structure
        &pi           // Pointer to PROCESS_INFORMATION structure
    )) {
        // Close process and thread handles to prevent resource leaks
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        std::cerr << "Error: Unable to launch app. Windows Error Code: " << GetLastError() << "\n";
    }
}

int main() {
    std::cout << "Scanning Windows Registry for registered apps...\n\n";
    std::vector<InstalledApp> apps = getRegistryApps();

    if (apps.empty()) {
        std::cout << "No registered applications found in the registry.\n";
        return 0;
    }

    // Display the generated menu
    for (size_t i = 0; i < apps.size(); ++i) {
        std::cout << i + 1 << ". " << apps[i].name << "\n";
    }

    std::cout << "\nChoose an application to launch (1-" << apps.size() << "): ";
    std::string input;
    std::getline(std::cin, input);

    try {
        int choice = std::stoi(input);
        if (choice >= 1 && choice <= static_cast<int>(apps.size())) {
            launchApp(apps[choice - 1].exePath);
        } else {
            std::cout << "Invalid choice. Please run again and select a number from the list.\n";
        }
    } catch (const std::invalid_argument&) {
        std::cout << "Invalid input. Please enter a valid index number.\n";
    }

    return 0;
}
