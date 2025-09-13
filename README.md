# Telegram Keylogger & Administration Bot

**Disclaimer: This project is strictly for educational and security research purposes. Unauthorized use on systems you do not own is illegal and unethical. The developers are not responsible for any misuse of this software.**

A stealthy Windows keylogger and system monitoring tool that uses a Telegram bot for remote command and control (C2). This demonstrates modern post-exploitation techniques, including persistence, data exfiltration, and communication over common web protocols to bypass firewalls.

---

## 🚨 Warning & Legal Notice

**PLEASE READ THIS CAREFULLY BEFORE PROCEEDING.**

-   **Illegal Activity:** Using this software to monitor users without their explicit consent is **illegal** in most countries. It violates laws related to computer fraud, privacy, and wiretapping.
-   **Educational Purpose:** This code is provided **for educational purposes only**. It is intended for:
    -   Security researchers to understand attack vectors.
    -   Penetration testers to use in authorized engagements.
    -   Developers to learn about Windows APIs and security.
-   **Ethical Use:** You must have **explicit, written permission** from the owner of any system on which you run this software.
-   **Detection:** This tool may be flagged as malware by antivirus software. Use it only in isolated, controlled environments (labs, VMs).

**By using this software, you acknowledge that you are solely responsible for its use and any consequences that result from it.**

---

## ✨ Features

-   **🔑 Keylogging:** Captures keystrokes, including special keys and handling for different keyboard layouts.
-   **📋 Clipboard Monitoring:** Logs text content copied to the clipboard, avoiding duplicates.
-   **🤖 Telegram C2:** Uses the Telegram Bot API as a communication channel, making it hard to block.
-   **👨‍💻 Admin Commands:** Remote control via Telegram commands.
-   **📷 Screenshot Capture:** Takes screenshots of the target's desktop on demand.
-   **🐚 Remote Shell:** Executes system commands and returns the output.
-   **ℹ️ System Information:** Retrieves details about the target machine (username, OS, etc.).
-   **⚙️ Builder Tool:** Easily configure and build custom client binaries with your Telegram API token.
-   **🎯 Persistence:** Automatically adds itself to the Windows Registry to run on startup.
-   **🛡️ Basic OPSEC:** Compiles to a hidden Windows application.

---

## 📦 Installation & Setup

### Prerequisites

-   **Windows OS** (for building and running the client)
-   **Visual Studio** (with C++ development tools and the "Developer Command Prompt")
-   A **Telegram** account

### 1. Create Your Telegram Bot

1.  Open Telegram and search for `@BotFather`.
2.  Send the `/newbot` command.
3.  Follow the instructions to set a **name** and **username** for your bot (e.g., `my_test_bot`).
4.  **Save the API Token** provided by BotFather. It will look like this: `110201543:AAHdqTcvCH1vGWJxfSeofSAs0K5PALDsaw`.
5.  Send a message to your new bot. Then, visit this URL in your browser to find your **Chat ID**:
    `https://api.telegram.org/bot<YOUR_API_TOKEN>/getUpdates`
    Look for the `"id"` field inside `"chat"`.

### 2. Build the Project

1.  **Clone the Repository:**
    ```bash
    git clone https://github.com/your_username/your_repo_name.git
    cd your_repo_name
    ```

2.  **Compile the Builder:**
    Open a **Developer Command Prompt for VS** and navigate to the project directory.
    ```bash
    cl /EHsc builder.cpp
    ```

3.  **Run the Builder:**
    Execute the builder and provide the information it asks for.
    ```bash
    builder.exe
    ```
    -   Enter your Telegram Bot API Token.
    -   Enter your Admin Chat ID.
    -   Choose an output name for the client (e.g., `WindowsUpdate.exe`).

4.  **Get Your Client:**
    The builder will compile a new `client.exe` (or your chosen name) with your credentials embedded. **This is the file you deploy.**

---

## 🖥️ Usage

1.  **Deploy the Client:** Execute the custom-built client binary on the target machine (ensure you have permission!).
2.  **Interact via Telegram:** Send commands to your bot in Telegram:

    | Command | Description | Example |
    | :--- | :--- | :--- |
    | `/log [lines]` | Fetches the last X lines of captured keystrokes. | `/log 50` |
    | `/clipboard` | Fetches the current text content of the clipboard. | `/clipboard` |
    | `/screenshot` | Takes a screenshot of the target's desktop. | `/screenshot` |
    | `/shell <command>` | Executes a system command and returns the output. | `/shell whoami` |
    | `/sysinfo` | Gets basic system information (OS, user, etc.). | `/sysinfo` |

---

## 🧠 How It Works

### Architecture
```mermaid
graph TD
    subgraph Attacker
        A[Telegram App] -->|Sends Commands| B(Telegram Bot);
        B -->|Receives Data| A;
    end

    subgraph Target Machine
        C[Client.exe] -->|Polls for Commands| B;
        C -->|Sends Logs/Data| B;
        C --> D[Keylogger Hook];
        C --> E[Clipboard Monitor];
    end

    style C fill:#ff9999
    style B fill:#90ee90
