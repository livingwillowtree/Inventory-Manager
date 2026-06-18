#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

#include "models.h"

struct InputReports {
    const std::string invalidIntType = "[Input Error]: Please enter a valid whole number.\n";
    const std::string invalidIntRange = "[Input Error]: Number is out of the allowed range.\n";
    const std::string invalidDoubleType = "[Input Error]: Please enter a valid decimal number.\n";
    const std::string invalidDoubleRange = "[Input Error]: Value is out of the allowed range.\n";
    const std::string invalidStrLength = "[Input Error]: Text length does not meet the range limits.\n";
    const std::string invalidStrPipePresent = "[Input Error]: Pipe ('|') was detected. Please avoid this character\n";
    const std::string invalidCharType = "[Input Error]: Please enter only a single character.\n";
    const std::string invalidCharVal = "[Input Error]: That choice is not recognized.\n";
    const std::string duplicateID = "[Input Error]: Product ID already exists. Please use a unique ID.\n";
    
} const imsg;

const int CRIT_THRESHOLD = 5;

const int WIDTH_ID = 8;
const int WIDTH_NAME = 50;
const int WIDTH_PRICE = 10;
const int WIDTH_STOCK = 8;
const int WIDTH_C_NAME = 40;
const int WIDTH_T_PRICE = 12;
const int WIDTH_STOCK_AVAILABILITY = 18;
const int WIDTH_CUSTOMER_NAME = 30;
const int WIDTH_TXN_ID = 10;

const int WIDTH_REC_DESC = 30;
const int WIDTH_REC_QTY = 10;
const int WIDTH_REC_PRICE = 12;
const int WIDTH_REC_TOTAL = 12;
const int WIDTH_REC_DATE = 25;

std::string getString(const std::string& prompt = "", int min = 1, int max = 999999);
double getDouble(const std::string& prompt = "", double min = 0.0, double max = 999999.9);
int getInt(const std::string& prompt = "", int min = 0, int max = 999999);
char getChar(const std::string& prompt = "", char min = 'A', char max = 'z');
bool validateString(const std::string& str);
bool validateDoubleStr(const std::string& str);
bool validateIntStr(const std::string& str);
bool isIdDuplicate(const std::string& id, const std::vector<ProductInfo>& inventory);
void clearScreen();

#endif