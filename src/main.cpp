#include <iostream>
#include <string>
#include <vector>

#include "models.h"
#include "utils.h"
#include "services.h"
#include "fileHandling.h"

void inventoryManagement(std::vector<ProductInfo>& inventory);
void stockMonitoring(const std::vector<ProductInfo>& inventory);
void salesAndTransaction(std::vector<ProductInfo>& inventory, std::vector<SaleReceipt>& salesLog);
bool isSure();

int main() {
    std::vector<ProductInfo> inventory;
    std::vector<SaleReceipt> salesLog;

    int status = loadInventory(inventory);
    std::cout << "\n\n";
    switch (status) {
        case fileStatus.SUCCESS:
            std::cout << "Inventory -> " << fmsg.successLoad;
            break;
        case fileStatus.WARNING_PARSING_ERRORS:
            std::cout << fmsg.parsedWithSkippedLines;
            break;
        case fileStatus.ERROR_FILE_NOT_READ:
            std::cout << fmsg.fileCantBeRead;
            return 1;
    }

    status = loadLogs(salesLog);
    switch (status) {
        case fileStatus.SUCCESS:
            std::cout << "Sales Log -> " << fmsg.successLoad;
            break;
        case fileStatus.ERROR_LOG_NOT_FOUND:
            std::cout << fmsg.fileCantBeRead;
            return 1;
    }

    std::vector<ProductInfo> tempInv = inventory;
    std::vector<SaleReceipt> tempLog = salesLog;

    bool isRunning = true;
    int choice = 0;

    while (isRunning) {
        displayMainMenu();
        choice = getInt("", 0, 4);

        switch (choice) {
            case 1:
                clearScreen();
                inventoryManagement(inventory);
                break;
            case 2:
                clearScreen();
                stockMonitoring(inventory);
                break;
            case 3:
                clearScreen();
                salesAndTransaction(inventory, salesLog);
                break;
            case 4:
                clearScreen();
                status = saveInventory(inventory);
                std::cout << "\n\n";
                switch (status) {
                    case fileStatus.SUCCESS :
                        std::cout << "Inventory -> " << fmsg.successSave;
                        tempInv = inventory;
                        break;
                    case fileStatus.ERROR_FILE_NOT_SAVED :
                        std::cout << "Inventory -> " << fmsg.savingFailed;
                        break;
                    case fileStatus.ERROR_WRITING :
                        std::cout << "Inventory -> " << fmsg.writingFailed;
                        break;
                    case fileStatus.ERROR_FILE_NOT_OPENED :
                        std::cout << "Inventory -> " << fmsg.fileNotOpened;
                        break;
                }
                status = saveLogs(salesLog);
                std::cout << "\n\n";
                switch (status) {
                    case fileStatus.SUCCESS :
                        std::cout << "Sales Log -> " << fmsg.successSave;
                        tempLog = salesLog;
                        break;
                    case fileStatus.ERROR_FILE_NOT_SAVED :
                        std::cout << "Sales Log -> " << fmsg.savingFailed;
                        break;
                    case fileStatus.ERROR_WRITING :
                        std::cout << "Sales Log -> " << fmsg.writingFailed;
                        break;
                    case fileStatus.ERROR_FILE_NOT_OPENED :
                        std::cout << "Sales Log -> " << fmsg.fileNotOpened;
                        break;
                }
                break;
            case 0:
                if ((tempInv == inventory && tempLog == salesLog) || isSure()) {
                    isRunning = false;
                    std::cout << "Exiting System... \n";
                }
                break;
        }
    }
    return 0;
}

void inventoryManagement(std::vector<ProductInfo>& inventory) {
    bool isRunning = true;
    int choice = 0;
    while (isRunning) {
        displayInvMngMenu();
        choice = getInt("", 0, 5);

        switch (choice) {
            case 1:
                displayProducts(inventory);
                break;
            case 2:
                searchProduct(inventory);
                break;
            case 3:
                updateProduct(inventory);
                break;
            case 4:
                addProduct(inventory);
                break;
            case 5:
                deleteProduct(inventory);
                break;
            case 0:
                isRunning = false;
                std::cout << "Exiting Product Management... \n\n";
                break;
        }
    }
}

void stockMonitoring(const std::vector<ProductInfo>& inventory) {
    bool isRunning = true;
    int choice = 0;
    while (isRunning) {
        displayStockMonMenu();
        choice = getInt("", 0, 3);

        switch (choice) {
            case 1:
                displayStock(inventory, 'a');
                break;
            case 2:
                displayStock(inventory, 'c');
                break;\
            case 3:
                displayStock(inventory, 's');
                break;
            case 0:
                isRunning = false;
                std::cout << "Exiting Stock Monitoring... \n\n";
                break;
        }
    }
}

void salesAndTransaction(std::vector<ProductInfo>& inventory, std::vector<SaleReceipt>& salesLog) {
    bool isRunning = true;
    int choice = 0;
    while (isRunning) {
        displaySalesMenu();
        choice = getInt("", 0, 3);

        switch (choice) {
            case 1:
                processOrder(inventory, salesLog);
                break;
            case 2:
                displaySalesLog(salesLog);
                break;
            case 3:
                displayAnalysisReport(inventory, salesLog);
                break;
            case 0:
                isRunning = false;
                std::cout << "Exiting Customer Orders... \n\n";
                break;
        }
    }
}

bool isSure() {
    char choice = getChar("[WARNING] Changes not saved. Exit anyways? ('Y'/'n')", 'A', 'z');
    return (choice == 'Y' || choice == 'y');
}
