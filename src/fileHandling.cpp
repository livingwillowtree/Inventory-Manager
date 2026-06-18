#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

#include "models.h"
#include "utils.h"
#include "fileHandling.h"

struct SaleInfo {
    std::string transactionID;
    std::string customerName;
    std::string date;
    double grandTotal;
    
    CartItem item;         // Captured item context packed nicely
    bool isValid = true;
};

SaleInfo parseSaleLine(const std::string& line);
ProductInfo parseInvLine(const std::string& productRawData);
std::string encodeProductData(const ProductInfo& product);

int loadInventory(std::vector<ProductInfo>& productInventory) {
    std::string productRawData;
    ProductInfo processedData;
    bool fileClean = true;

    std::ifstream readFile(INVENTORY_FILE_PATH);
    if (!readFile.is_open()) return fileStatus.ERROR_FILE_NOT_READ;

    while (std::getline(readFile, productRawData)) {
        if (productRawData.empty()) continue;

        processedData = parseInvLine(productRawData);

        if (processedData.ID == "[ERROR]") {
            std::cout << fmsg.corruptLineSkipped;
            std::cout << " -> Raw Line : " << productRawData << "\n";
            fileClean = false;
            continue;
        }

        productInventory.push_back(processedData);
    }
    readFile.close();

    if (!fileClean) return fileStatus.WARNING_PARSING_ERRORS;
    return fileStatus.SUCCESS;
}

ProductInfo parseInvLine(const std::string& productRawData) {
    std::string ID, name;
    double price;
    int stockQnty;

    std::stringstream ss(productRawData);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    if (tokens.size() != 4) {
        return {"[ERROR]", "", 0.0, 0};
    }

    ID = tokens[0];
    name = tokens[1];

    if (!validateDoubleStr(tokens[2]) || !validateIntStr(tokens[3])) {
        return {"[ERROR]", "", 0.0, 0};
    }

    price = std::stod(tokens[2]);
    stockQnty = std::stoi(tokens[3]);

    return {ID, name, price, stockQnty};
}

int saveInventory(const std::vector<ProductInfo>& productInventory) {
    std::ofstream writeFile(INVENTORY_FILE_PATH + ".tmp");
    if (!writeFile.is_open()) return fileStatus.ERROR_FILE_NOT_OPENED;

    for (const auto& product : productInventory) {
        writeFile << encodeProductData(product) << "\n";
        if (writeFile.bad()) {
            writeFile.close();
            return fileStatus.ERROR_WRITING;
        }
    }

    writeFile.close();

    std::error_code ec;
    std::filesystem::rename(INVENTORY_FILE_PATH + ".tmp", INVENTORY_FILE_PATH, ec);
    if (ec) {
        return fileStatus.ERROR_FILE_NOT_SAVED;
    }

    return fileStatus.SUCCESS;
}

std::string encodeProductData(const ProductInfo& product) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << product.ID << "|" << product.name << "|" << product.price << "|" << product.stockQnty;
    return ss.str();
}

int loadLogs(std::vector<SaleReceipt>& salesLog) {
    std::string rawLine;
    std::ifstream readFile(SALES_FILE_PATH);
    // Blocks execution if sales log file path cannot be tracked/read successfully
    if (!readFile.is_open()) return fileStatus.ERROR_FILE_NOT_READ;

    while (std::getline(readFile, rawLine)) {
        if (rawLine.empty()) continue;

        SaleInfo parsed = parseSaleLine(rawLine);
        if (!parsed.isValid) continue;

        // If this item belongs to the transaction we are currently building, append it to the same vector
        if (!salesLog.empty() && salesLog.back().transactionID == parsed.transactionID) {
            salesLog.back().itemsBought.push_back(parsed.item);
        } else {
            // Otherwise, set up a new base record block
            SaleReceipt newReceipt;
            newReceipt.transactionID = parsed.transactionID;
            newReceipt.customerName = parsed.customerName;
            newReceipt.date = parsed.date;
            newReceipt.grandTotal = parsed.grandTotal;
            // Missing payment information is initialized as safe defaults
            newReceipt.customerPayment = 0.0;
            newReceipt.change = 0.0;
            
            newReceipt.itemsBought.push_back(parsed.item);
            salesLog.push_back(newReceipt);
        }
    }

    readFile.close();
    return fileStatus.SUCCESS;
}

SaleInfo parseSaleLine(const std::string& line) {
    SaleInfo result;
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    // Refactored strict bounds check down to your exact 8 elements layout pattern
    if (tokens.size() != 8 || !validateIntStr(tokens[4]) || !validateDoubleStr(tokens[5]) || !validateDoubleStr(tokens[6]) || !validateDoubleStr(tokens[7])) {
        result.isValid = false;
        return result;
    }

    result.transactionID = tokens[0];
    result.customerName = tokens[1];
    result.date = tokens[2];
    result.grandTotal = std::stod(tokens[7]);

    // Capture the line item snapshot variables safely inside the item object block
    result.item.ID = tokens[3];
    result.item.name = ""; // Name string is stripped out to match parameters
    result.item.orderQty = std::stoi(tokens[4]);
    result.item.price = std::stod(tokens[5]);
    result.item.total = std::stod(tokens[6]);

    return result;
}

int saveLogs(const std::vector<SaleReceipt>& salesLog) {
    std::ofstream writeFile(SALES_FILE_PATH + ".tmp");
    if (!writeFile.is_open()) return fileStatus.ERROR_FILE_NOT_OPENED;

    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);

    for (const auto& receipt : salesLog) {
        for (const auto& item : receipt.itemsBought) {
            ss.str("");
            ss.clear();

            // Writes the trimmed down 8-token transaction architecture layout
            ss << receipt.transactionID << "|"
               << receipt.customerName << "|"
               << receipt.date << "|"
               << item.ID << "|"
               << item.orderQty << "|"
               << item.price << "|"
               << item.total << "|"
               << receipt.grandTotal;

            writeFile << ss.str() << "\n";
            if (writeFile.bad()) {
                writeFile.close();
                return fileStatus.ERROR_WRITING;
            }
        }
    }
    writeFile.close();

    std::error_code ec;
    std::filesystem::rename(SALES_FILE_PATH + ".tmp", SALES_FILE_PATH, ec);
    if (ec) return fileStatus.ERROR_FILE_NOT_SAVED;

    return fileStatus.SUCCESS;
}