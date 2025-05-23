// #include "../include/ui.h"
// #include <iostream>
// #include <conio.h>

// using namespace std;

// void showMenu() {
//     cout << "========================\n";
//     cout << "TRANSACTION SYSTEM\n";
//     cout << "1 LOGIN\n";
//     cout << "2 REGISTER\n";
//     cout << "3 REGISTER FOR MANAGER\n";
//     cout << "PRESS 1/2/3\n";
//     cout << "========================\n";
// }

// void showTransactionMenu() {
//     cout << "========================\n";
//     cout << "TRANSACTION SYSTEM\n";
//     cout << "1 CHANGE PASSWORD\n";
//     cout << "2. Chuyển tiền" << endl;
//     cout << "...\n";
//     cout << "PRESS 1/...\n";
//     cout << "========================\n";
// }

// char getMenuChoice() {
//     return _getch();
// }

// void showRegisterScreen() {
//     cout << ">> REGISTER SYSTEM <<\n";
// }

// void showLoginScreen() {
//     cout << ">> LOGIN SYSTEM <<\n";
// }

// void showRegisterScreenForMnger() {
// 	cout << ">> REGISTER SYSTEM FOR MANAGER <<\n";
// }
// string getInput(const string& prompt) {
//     cout << prompt;
//     string input;
//     getline(cin, input);
//     return input;
// }

// void showMessage(const string& message) {
//     cout << message << "\n";
// }

// void showChangePasswordScreen() {
// 	cout << ">> CHANGE YOUR INFORMATION <<\n";
// }

// void showChanginPassScreen() {
//     cout << ">> CHANGING PASSWORD <<\n";
// }

#include "../include/ui.h"
#include <bits/stdc++.h>
#include <conio.h>
#include <windows.h>

using namespace std;
string formatCurrency(double amount) {
    stringstream ss;
    // Dùng locale mặc định để tránh crash
    ss.imbue(locale(""));  

    ss << fixed << setprecision(2) << amount;
    string result = ss.str();

    if (result.find(".00") != string::npos)
        result = result.substr(0, result.length() - 3);

    return result + " VND";
}

void showMenu() {
    printHeader("MAIN MENU");
    cout << "\033[1;33m";
    cout << "\n[1] Login\n";
    cout << "[2] Register\n";
    cout << "[3] Manager Registration\n";
    cout << "\nPlease select 1 / 2 / 3: ";
    cout << "\033[0m";
}

void showTransactionMenu() {
    printHeader("TRANSACTION MENU");
    cout << "\033[1;33m";
    cout << "[1] Change Password\n";
    cout << "[2] Transfer Money\n";
    cout << "[3] View Balance\n";
    cout << "[4] Logout" << endl;
    cout << "Select [1] / [2] / [3] / [4]: ";
    cout << "\033[0m";
}

char getMenuChoice() {
    return _getch();
}

void showRegisterScreen() {
    printHeader("REGISTER");
}

void showLoginScreen() {
    printHeader("LOGIN");
}

void showRegisterScreenForMnger() {
	printHeader("MANAGER REGISTRATION");
}

string getInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}

void showMessage(const string& message) {
    cout << message << "\n";
}

void showChangePasswordScreen() {
	printHeader("UPDATE INFORMATION");
}

void showChanginPassScreen() {
    printHeader("CHANGE PASSWORD");
}

void showMngerMenu() {
    printHeader("MANAGER MENU");
    cout << "\033[1;33m";
    cout << "[1] Create User Account\n";
    cout << "[2] Edit Information\n";
    cout << "\nPlease select 1 / 2: ";
    cout << "\033[0m";

}

int visualLength(const std::string& s);  


void printHeader(const string& title) {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    const int totalWidth = 40;
    int titleLen = visualLength(title);
    int paddingLeft = (totalWidth - titleLen) / 2;
    int paddingRight = totalWidth - titleLen - paddingLeft;

    cout << "\033[1;36m";
    cout << "╔";
    for (int i = 0; i < totalWidth; ++i) cout << "═";
    cout << "╗\n║";
    for (int i = 0; i < paddingLeft; ++i) cout << " ";
    cout << title;
    for (int i = 0; i < paddingRight; ++i) cout << " ";
    cout << "║\n╚";
    for (int i = 0; i < totalWidth; ++i) cout << "═";
    cout << "╝\n\033[0m";
}

int visualLength(const string& s) {
    int count = 0;
    for (size_t i = 0; i < s.length(); ) {
        if ((s[i] & 0x80) == 0) {
            ++count;
            ++i;
        } else if ((s[i] & 0xE0) == 0xC0) {
            ++count;
            i += 2;
        } else if ((s[i] & 0xF0) == 0xE0) {
            ++count;
            i += 3;
        } else {
            ++count;
            ++i;
        }
    }
    return count;
}
