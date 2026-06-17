#include <iostream>

#include "utils.h"

std::string getString(const std::string& prompt, int min, int max) {
    std::string input;
    while (true) {
        std::cout << prompt << "\n >> ";
        std::getline(std::cin, input);
        if ((int)input.length() < min || (int)input.length() > max) {
            std::cout << imsg.invalidStrLength;
            continue;
        }
        if (!validateString(input)) {
            std::cout << imsg.invalidStrPipePresent;
            continue;
        }
        return input;
    }
}

double getDouble(const std::string& prompt, double min, double max) {
    std::string strInput;
    while (true) {
        std::cout << prompt << "\n >> ";
        std::getline(std::cin, strInput);
        if (!validateDoubleStr(strInput)) {
            std::cout << imsg.invalidDoubleType;
            continue;
        }
        double val = std::stod(strInput);
        if (val < min || val > max) {
            std::cout << imsg.invalidDoubleRange;
            continue;
        }
        return val;
    }
}

int getInt(const std::string& prompt, int min, int max) {
    std::string strInput;
    while (true) {
        if (!prompt.empty()) {
            std::cout << prompt << "\n";
        }
        std::cout << " >> ";
        std::getline(std::cin, strInput);
        if (!validateIntStr(strInput)) {
            std::cout << imsg.invalidIntType;
            continue;
        }
        int val = std::stoi(strInput);
        if (val < min || val > max) {
            std::cout << imsg.invalidIntRange;
            continue;
        }
        return val;
    }
}

char getChar(const std::string& prompt, char min, char max) {
    std::string strInput;
    while (true) {
        std::cout << prompt << "\n >> ";
        std::getline(std::cin, strInput);
        if ((int)strInput.length() != 1) {
            std::cout << imsg.invalidCharType;
            continue;
        }
        char chInput = strInput[0];
        if (chInput < min || chInput > max) {
            std::cout << imsg.invalidCharVal;
            continue;
        }
        return chInput;
    }
}

bool isIdDuplicate(const std::string& id, const std::vector<ProductInfo>& inventory) {
    for (const auto& product : inventory) {
        if (product.ID == id) return true;
    }
    return false;
}

bool validateString(const std::string& str) {
    for (char c : str) {
        if (c == '|') return false;
    }
    return true;
}

bool validateDoubleStr(const std::string& str) {
    if (str.empty() || str == ".") return false;
    bool periodFound = false;
    for (char c : str) {
        if (c == '.') {
            if (!periodFound) {
                periodFound = true;
                continue;
            } else return false;
        }
        if (c < '0' || c > '9') return false;
    }
    return true;
}

bool validateIntStr(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (c < '0' || c > '9') return false;
    }
    return true;
}