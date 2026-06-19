#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>


#include "services.h"
#include "utils.h"
#include "fileHandling.h"

void printInventoryTableSeparator();
void printInventoryTableHeader();
void printCartTableSeparator();
void printSalesLogSeparator(char symbol);
void printCartTableHeader();
void printProductInfo(const ProductInfo& p);
void printStockTableSeparator();
void printStockTableHeader();
void printStockInfo(const ProductInfo& p);
void printCartInfo(const CartItem& c);
void printReceiptHeader();
void printReceiptItem(const CartItem& c);

void addToCart(std::vector<ProductInfo>& products, std::vector<CartItem>& cart);
void removeFromCart(std::vector<CartItem>& cart);
void viewCart(const std::vector<CartItem>& cart);
char checkout(std::vector<CartItem>& cart, std::vector<ProductInfo>& inventory, std::vector<SaleReceipt>& salesLog);
std::string generateNextTransactionID(const std::vector<SaleReceipt>& logs);
std::string getCurrentDate();


void addProduct(std::vector<ProductInfo>& inventory) {
    std::string ID;
    std::string name;
    double price;
    int stockQnty;

    while (true) {
        ID = getString("Enter Product ID ('cancel' to cancel) : ", 4, 6);
        if (ID == "cancel") return;
        if (isIdDuplicate(ID, inventory)) {
            std::cout << imsg.duplicateID;
            continue;
        }
        break;
    }
    name = getString("Enter Product Name ('cancel' to cancel) :", 1, 100);
    if (name == "cancel") return;
    price = getDouble("Enter Product Price (0.0 to cancel)", 0.0);
    if (price == 0.0) return;
    stockQnty = getInt("Enter Stock Quantity (0 to cancel)", 0);
    if (stockQnty == 0) return;

    inventory.push_back({ID, name, price, stockQnty});
    std::cout << "\nAdd Product -> " << smsg.operationSuccess;
}

void updateProduct(std::vector<ProductInfo>& products) {
    std::string targetID;
    ProductInfo* targetProduct = nullptr;
    bool found = false;
    char choice;

    std::cout << "\n--- Update Product System ---\n";
    if (products.empty()) {
        std::cout << smsg.searchUnavailable;
        return;
    }

    targetID = getString("\nEnter Target Product ID ('cancel' to cancel): ", 4, 6);
    if (targetID == "cancel") return;

    for (auto& p : products) {
        if (p.ID == targetID) {
            targetProduct = &p;
            found = true;
            break;
        }
    }

    if (!found) {
        std::cout << smsg.productNotFound;
        return;
    }

    do {
        std::cout << "\n[Product Profile Loaded]\n";
        printInventoryTableHeader();
        printProductInfo(*targetProduct);
        printInventoryTableSeparator();

        std::cout << "\nSelect Field to Modify:\n"
                  << "[1] Product Name\n"
                  << "[2] Product Price\n"
                  << "[3] Stock Quantity\n"
                  << "[0] Commit\n";

        choice = getChar("", '0', '3');

        switch (choice) {
            case '1': {
                std::string newName = getString("Enter replacement product name:", 1, 100);
                targetProduct->name = newName;
                break;
            }
            case '2': {
                double newPrice = getDouble("Enter new product price:", 0.01);
                targetProduct->price = newPrice;
                break;
            }
            case '3': {
                int replenishment = getInt("Enter new stock quantity:", 1);
                targetProduct->stockQnty = replenishment;
                break;
            }
            case '0':
                std::cout << "\nExiting...\n";
                break;
        }
    } while (choice != '0');
}

void searchProduct(const std::vector<ProductInfo>& products) {
    std::cout << "\n--- Inventory Lookup System ---\n";
    if (products.empty()) {
        std::cout << smsg.searchUnavailable;
        return;
    }

    std::cout << "Select search mode:\n"
              << "[1] Strict ID Matching\n"
              << "[2] Loose Name Search\n"
              << "[0] Exit\n";

    char mode = getChar("", '0', '2');

    if (mode == '1') {
        std::string targetID = getString("\nEnter Target Product ID: ", 4, 6);
        bool found = false;
        for (const auto& p : products) {
            if (p.ID == targetID) {
                std::cout << "\n[Match Verified]\n";
                printInventoryTableHeader();
                printProductInfo(p);
                printInventoryTableSeparator();
                found = true;
                break;
            }
        }
        if (!found) std::cout << smsg.productNotFound;
    } else if (mode == '2') {
        std::string keyword = getString("\nEnter Name: ", 1, 100);
        std::string lowerKeyword = keyword;
        std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);

        std::cout << "\n[Result]\n";
        printInventoryTableHeader();
        bool found = false;

        for (const auto& p : products) {
            std::string lowerName = p.name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

            if (lowerName.find(lowerKeyword) != std::string::npos) {
                found = true;
                printProductInfo(p);
            }
        }
        printInventoryTableSeparator();
        if (!found) std::cout << smsg.nameNotFound;
    }
    std::cout << "\nExiting...\n";
}

void deleteProduct(std::vector<ProductInfo>& products) {
    std::cout << "\n--- Delete Product Record ---\n";

    if (products.empty()) {
        std::cout << smsg.inventoryEmpty;
        return;
    }

    std::string targetID = getString("\nEnter Product ID to delete ('cancel' to cancel): ", 4, 6);
    if (targetID == "cancel") return;

    bool found = false;
    int id = 0;

    for (const ProductInfo& p : products) {
        if (p.ID == targetID) {
            found = true;
            std::cout << "\n[Product Found]\n";
            break;
        }
        id++;
    }

    if (!found) {
        std::cout << smsg.productNotFound;
        return;
    }

    printInventoryTableHeader();
    printProductInfo(products[id]);
    printInventoryTableSeparator();

    char confirm = getChar("Confirm deletion? (y/n): ", 'A', 'z');

    if (std::tolower(confirm) == 'y') {
        products.erase(products.begin() + id);
        std::cout << "\nDelete Record -> " << smsg.operationSuccess;
    } else {
        std::cout << "\nDelete Record -> " << smsg.operationCancelled;
    }
}

void displayAnalysisReport(const std::vector<ProductInfo>& inventory, const std::vector<SaleReceipt>& salesLog) {
    std::cout << "\n==================================================";
    std::cout << "\n               INVENTORY ANALYSIS REPORT          ";
    std::cout << "\n==================================================\n";

    int outOfStockCount = 0;
    int lowStockCount = 0;
    
    for (const auto& product : inventory) {
        if (product.stockQnty == 0) outOfStockCount++;
        else if (product.stockQnty <= CRIT_THRESHOLD) lowStockCount++; 
    }

    std::cout << "--- Inventory Warehouse State ---\n";
    std::cout << " Unique Products Tracked : " << inventory.size() << "\n";
    std::cout << " Out of Stock Items [!]  : " << outOfStockCount << "\n";
    std::cout << " Critically Low Items    : " << lowStockCount << "\n\n";

    if (salesLog.empty()) {
        std::cout << "--- Financial & Sales Performance ---\n";
        std::cout << " [Notice]: No transaction logs found. Revenue metrics unavailable.\n";
        std::cout << "==================================================\n";
        return;
    }

    double totalRevenue = 0.0;
    int totalUnitsSold = 0;
    
    std::map<std::string, int> productQuantities;
    std::map<std::string, double> productRevenue;

    for (const auto& receipt : salesLog) {
        totalRevenue += receipt.grandTotal;
        
        for (const auto& item : receipt.itemsBought) {
            totalUnitsSold += item.orderQty;
            productQuantities[item.ID] += item.orderQty;
            productRevenue[item.ID] += item.total;
        }
    }

    double avgTicket = totalRevenue / salesLog.size();

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "--- Financial & Sales Performance ---\n";
    std::cout << " Total Revenue Generated : " << totalRevenue << "\n";
    std::cout << " Total Invoices Processed: " << salesLog.size() << "\n";
    std::cout << " Average Transaction Value    : " << avgTicket << "\n";
    std::cout << " Total Stock Units Sold  : " << totalUnitsSold << "\n\n";

    std::string bestSellerID = "";
    int maxQty = 0;
    std::string topEarnerID = "";
    double maxRevenue = 0.0;

    for (const auto& [id, qty] : productQuantities) {
        if (qty > maxQty) {
            maxQty = qty;
            bestSellerID = id;
        }
    }

    for (const auto& [id, rev] : productRevenue) {
        if (rev > maxRevenue) {
            maxRevenue = rev;
            topEarnerID = id;
        }
    }

    std::cout << "--- Product Performance Highlights ---\n";
    if (!bestSellerID.empty()) {
        std::cout << " Best Selling Item (Volume) : Product ID " << bestSellerID 
                  << " (" << maxQty << " units sold)\n";
    }
    if (!topEarnerID.empty()) {
        std::cout << " Highest Revenue Generator  : Product ID " << topEarnerID 
                  << " (" << maxRevenue << " generated)\n";
    }
    std::cout << "==================================================\n";
}

void processOrder(std::vector<ProductInfo>& products, std::vector<SaleReceipt>& salesLog) {
    if (products.empty()) {
        std::cout << smsg.inventoryEmpty;
        return;
    }

    std::vector<CartItem> cart;
    char menuChoice;

    std::cout << std::fixed << std::setprecision(2);

    do {
        std::cout << "\n=== CUSTOMER ORDERING SYSTEM ===\n";
        std::cout << "a. Add Item to Cart\n";
        std::cout << "b. Remove Item from Cart\n";
        std::cout << "c. View Current Cart Status\n";
        std::cout << "d. Checkout and Finalize Transaction\n";
        std::cout << "e. Cancel Order Session\n";

        menuChoice = getChar("", 'a', 'e');

        switch (menuChoice) {
            case 'a':
                addToCart(products, cart);
                break;
            case 'b':
                removeFromCart(cart);
                break;
            case 'c':
                if (cart.empty()) {
                    std::cout << smsg.cartEmpty;
                } else {
                    double runningTotal = 0.0;
                    for (const auto& item : cart) runningTotal += item.total;
                    displayCart(cart, runningTotal);
                }
                break;
            case 'd':
                if (cart.empty()) {
                    std::cout << smsg.cartEmpty;
                } else {
                    char outcome = checkout(cart, products, salesLog);
                    if (outcome == 'd') {
                        menuChoice = 'e';
                    }
                }
                break;
            case 'e':
                std::cout << "\nStopping session...\n";
                cart.clear();
                break;
            default:
                std::cout << "\nHow.\n";
                break;
        }
    } while (menuChoice != 'e');
}

void addToCart(std::vector<ProductInfo>& products, std::vector<CartItem>& cart) {
    displayProducts(products);

    std::string searchID = getString("\nEnter product ID ('cancel' to cancel):", 4, 6);
    if (searchID == "cancel") return;

    ProductInfo* targetProduct = nullptr;
    for (auto& p : products) {
        if (p.ID == searchID) {
            targetProduct = &p;
            break;
        }
    }

    if (!targetProduct) {
        std::cout << smsg.productNotFound;
        return;
    }

    if (targetProduct->stockQnty <= 0) {
        std::cout << smsg.outOfStock;
        return;
    }

    std::cout << "\nSelected Product : " << targetProduct->name << " | Available Allocation: " << targetProduct->stockQnty << "\n";
    int requestedUnits = getInt("Input amount:", 1, targetProduct->stockQnty);

    bool alreadyInCart = false;
    for (auto& cartLine : cart) {
        if (cartLine.ID == targetProduct->ID) {
            if (cartLine.orderQty + requestedUnits > targetProduct->stockQnty) {
                std::cout << smsg.lowAllocation;
                return;
            }
            cartLine.orderQty += requestedUnits;
            cartLine.total = cartLine.orderQty * cartLine.price;
            alreadyInCart = true;
            break;
        }
    }

    if (!alreadyInCart) {
        CartItem itemToInsert;
        itemToInsert.ID = targetProduct->ID;
        itemToInsert.name = targetProduct->name;
        itemToInsert.price = targetProduct->price;
        itemToInsert.orderQty = requestedUnits;
        itemToInsert.total = requestedUnits * targetProduct->price;
        cart.push_back(itemToInsert);
    }

    std::cout << "\nAdd to cart -> " << smsg.operationSuccess;
}

void removeFromCart(std::vector<CartItem>& cart) {
    if (cart.empty()) {
        std::cout << smsg.cartEmpty;
        return;
    }

    double runningTotal = 0.0;
    for (const auto& item : cart) runningTotal += item.total;
    displayCart(cart, runningTotal);

    std::string targetID = getString("\nEnter the Product ID:", 4, 6);

    auto iteratorIndex = std::find_if(cart.begin(), cart.end(), [&](const CartItem& c) {
        return c.ID == targetID;
    });

    if (iteratorIndex != cart.end()) {
        cart.erase(iteratorIndex);
        std::cout << "\nRemove item -> " << smsg.operationSuccess;
    } else {
        std::cout << smsg.productNotFound;
    }
}

char checkout(std::vector<CartItem>& cart, std::vector<ProductInfo>& inventory, std::vector<SaleReceipt>& salesLog) {
    double totalBillAmount = 0.0;
    for (const auto& lineItem : cart) {
        totalBillAmount += lineItem.total;
    }

    std::cout << "\n=== TRANSACTION BILL EVALUATION SUMMARY ===\n";
    double alternativeCalculatedTotal = 0.0;
    for (const auto& item : cart) alternativeCalculatedTotal += item.total;
    displayCart(cart, alternativeCalculatedTotal);

    std::string clientNameStr = getString("Enter customer billing identity :", 1, 20);
    double customerCashPayload = getDouble("Input received payment:", totalBillAmount, 9999999.9);

    double numericChangeReturn = customerCashPayload - totalBillAmount;

    for (const auto& activeCartUnit : cart) {
        for (auto& realWarehouseProduct : inventory) {
            if (realWarehouseProduct.ID == activeCartUnit.ID) {
                realWarehouseProduct.stockQnty -= activeCartUnit.orderQty;
                break;
            }
        }
    }

    SaleReceipt receipt;
    receipt.transactionID = generateNextTransactionID(salesLog);
    receipt.customerName = clientNameStr;
    receipt.itemsBought = cart;
    receipt.grandTotal = totalBillAmount;
    receipt.customerPayment = customerCashPayload;
    receipt.change = numericChangeReturn;
    receipt.date = getCurrentDate();

    std::cout << "\n" << std::string(WIDTH_REC_DESC + WIDTH_REC_QTY + WIDTH_REC_PRICE + WIDTH_REC_TOTAL + 13, '=') << "\n";
    std::cout << std::right << std::setw((WIDTH_REC_DESC + WIDTH_REC_QTY + WIDTH_REC_PRICE + WIDTH_REC_TOTAL + 13)/2+14) << "OFFICIAL INVOICE TRANSACTION\n";
    std::cout << std::string(WIDTH_REC_DESC + WIDTH_REC_QTY + WIDTH_REC_PRICE + WIDTH_REC_TOTAL + 13, '=') << "\n";
    std::cout << "ID Track:   " << receipt.transactionID << "\n";
    std::cout << "Timestamp:  " << receipt.date << "\n";
    std::cout << "Client:     " << receipt.customerName << "\n";
 
    printReceiptHeader();
    for (const auto& item : receipt.itemsBought) {
        printReceiptItem(item);
    }
    std::cout << " " << std::string(WIDTH_REC_DESC + WIDTH_REC_QTY + WIDTH_REC_PRICE + WIDTH_REC_TOTAL + 13 - 2, '-') << "\n";

    std::cout << std::left << std::setw(WIDTH_REC_DESC + WIDTH_REC_QTY + 5) << "| Net Grand Total :"
              << " | " << std::right << std::setw(WIDTH_REC_PRICE + WIDTH_REC_TOTAL + 3) << receipt.grandTotal << " |\n";
    std::cout << std::left << std::setw(WIDTH_REC_DESC + WIDTH_REC_QTY + 5) << "| Customer Payment :"
              << " | " << std::right << std::setw(WIDTH_REC_PRICE + WIDTH_REC_TOTAL + 3) << receipt.customerPayment << " |\n";
    std::cout << std::left << std::setw(WIDTH_REC_DESC + WIDTH_REC_QTY + 5) << "| Change Return :"
              << " | " << std::right << std::setw(WIDTH_REC_PRICE + WIDTH_REC_TOTAL + 3) << receipt.change << " |\n";
    std::cout << std::string(WIDTH_REC_DESC + WIDTH_REC_QTY + WIDTH_REC_PRICE + WIDTH_REC_TOTAL + 13, '=') << "\n";

    salesLog.push_back(receipt);
    cart.clear();

    return 'd';
}

std::string generateNextTransactionID(const std::vector<SaleReceipt>& logs) {
    if (logs.empty()) {
        return "TXN-1001";
    }

    std::string lastID = logs.back().transactionID;

    if (lastID.rfind("TXN-", 0) == 0) {
        std::string numericPart = lastID.substr(4);
        int nextNumber = std::stoi(numericPart) + 1;
        return "TXN-" + std::to_string(nextNumber);
    }

    return "TXN-1001";
}

std::string getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);
    std::tm time_info;

#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&time_info, &current_time);
#else
    localtime_r(&current_time, &time_info);
#endif

    std::stringstream bufferStringStream;
    bufferStringStream << std::put_time(&time_info, "%Y-%m-%d %H:%M:%S");
    return bufferStringStream.str();
}

void displayMainMenu() {
    std::string mainMenu = "\n===== PRODUCT INVENTORY && CUSTOMER ORDERING SYSTEM =====\n\n"
                           "      [1] Inventory Management\n"
                           "      [2] Stock Monitoring\n"
                           "      [3] Sales & Transaction\n"
                           "      [4] Save \n"
                           "      [0] Exit";
    std::cout << mainMenu << std::endl;
}

void displayInvMngMenu() {
    std::string prodMngMenu = "\n--- IVENTORY MANAGEMENT ---\n\n"
                              "    [1] Display Available products\n"
                              "    [2] Search Product\n"
                              "    [3] Update Product Info\n"
                              "    [4] Add Product Record\n"
                              "    [5] Delete Product Record\n"
                              "    [0] Exit";
    std::cout << prodMngMenu << std::endl;
}

void displayStockMonMenu() {
    std::string stockMonMenu = "\n--- STOCK MONITORING ---\n\n"
                               "    [1] Display All\n"
                               "    [2] Display Critical\n"
                               "    [3] Sort by Stock\n"
                               "    [0] Exit";
    std::cout << stockMonMenu << std::endl;
}

void displaySalesMenu() {
    std::string customerMenu = "\n--- SALES & TRANSACTION ---\n\n"
                               "    [1] Process New Order\n"
                               "    [2] View Sales Log\n"
                               "    [3] Get Analysis Report\n"
                               "    [0] Exit";
    std::cout << customerMenu << std::endl;
}

void displayProducts(const std::vector<ProductInfo>& products) {
    if (products.empty()) {
        std::cout << smsg.inventoryEmpty;
        return;
    }
    std::cout << "\n";
    printInventoryTableHeader();
    for (const auto& product : products) {
        if(product.stockQnty > 0)
            printProductInfo(product);
    }
    printInventoryTableSeparator();
}


void displayStock(const std::vector<ProductInfo>& products, char method) {
    if (products.empty()) {
        std::cout << smsg.inventoryEmpty;
        return;
    }
    std::cout << "\n";
    printStockTableHeader();

    std::vector<ProductInfo> displayList = products;

    if (method=='s') {
        std::sort(displayList.begin(), displayList.end(), [](const ProductInfo& a, const ProductInfo& b) {
            return a.stockQnty < b.stockQnty;
        });
    }

    bool criticalItemsExist = false;
    for (const auto& product : displayList) {
        if (method=='c') {
            if (product.stockQnty <= CRIT_THRESHOLD) {
                printStockInfo(product);
                criticalItemsExist = true;
            }
        } else {
            printStockInfo(product);
        }
    }

    if (method=='c' && !criticalItemsExist) {
        std::cout << "| " << std::left
                  << std::setw(WIDTH_ID + WIDTH_C_NAME + WIDTH_STOCK + WIDTH_STOCK_AVAILABILITY + 7)
                  << smsg.criticalStockNone << " |\n";
    }

    printStockTableSeparator();
}

void displayCart(const std::vector<CartItem>& cart, const double total) {
    if (cart.empty()) {
        std::cout << smsg.cartEmpty;
        return;
    }
    std::cout << "\n";
    printCartTableHeader();
    for (const auto& item : cart) {
        printCartInfo(item);
    }
    printCartTableSeparator();
    std::cout << std::right << std::setw(WIDTH_ID + WIDTH_C_NAME + WIDTH_PRICE + WIDTH_STOCK + 8) << "Grand Total: "
              << std::fixed << std::setprecision(2) << std::setw(WIDTH_T_PRICE) << total << "\n";
}

void displaySalesLog(const std::vector<SaleReceipt>& salesLog) {
    if (salesLog.empty()) {
        std::cout << smsg.noLogsFound;
        return;
    }

    std::cout << "\n";
    printSalesLogSeparator('=');
    std::cout << std::left << std::setw(WIDTH_TXN_ID) << "ID"
              << std::setw(WIDTH_CUSTOMER_NAME) << "Name"
              << std::setw(WIDTH_REC_DATE) << "Date"
              << std::setw(WIDTH_ID) << "Itm ID"
              << std::right << std::setw(WIDTH_STOCK) << "Qty"
              << std::setw(WIDTH_T_PRICE) << "Total" << "\n";
    printSalesLogSeparator('=');

    for (const auto& receipt : salesLog) {
        bool isFirstItem = true;

        for (const auto& item : receipt.itemsBought) {
            if (isFirstItem) {
                std::cout << std::left << std::setw(WIDTH_TXN_ID) << receipt.transactionID
                          << std::setw(WIDTH_CUSTOMER_NAME) << receipt.customerName
                          << std::setw(WIDTH_REC_DATE) << receipt.date;
                isFirstItem = false;
            } else {
                std::cout << std::left << std::setw(WIDTH_TXN_ID) << ""
                          << std::setw(WIDTH_CUSTOMER_NAME) << ""
                          << std::setw(WIDTH_REC_DATE) << "";
            }  

            std::cout << std::left << std::setw(WIDTH_ID) << item.ID
                      << std::right << std::setw(WIDTH_STOCK) << item.orderQty
                      << std::setw(WIDTH_T_PRICE) << std::fixed << std::setprecision(2) << item.total << "\n";
        }
        std::cout << std::left << std::setw(WIDTH_TXN_ID + WIDTH_CUSTOMER_NAME + WIDTH_REC_DATE + WIDTH_ID) << ""
                  << std::right << std::setw(WIDTH_STOCK) << "Sum:"
                  << std::setw(WIDTH_T_PRICE) << receipt.grandTotal << "\n";
        printSalesLogSeparator('-');
    }
}

void printInventoryTableSeparator() {
    std::cout << "+" << std::string(WIDTH_ID + 2, '-')
              << "+" << std::string(WIDTH_NAME + 2, '-')
              << "+" << std::string(WIDTH_PRICE + 2, '-')
              << "+" << std::string(WIDTH_STOCK + 2, '-') << "+\n";
}

void printInventoryTableHeader() {
    printInventoryTableSeparator();
    std::cout << "| " << std::left << std::setw(WIDTH_ID) << "ID"
              << " | " << std::setw(WIDTH_NAME) << "Product Name"
              << " | " << std::right << std::setw(WIDTH_PRICE) << "Price"
              << " | " << std::setw(WIDTH_STOCK) << "Stock" << " |\n";
    printInventoryTableSeparator();
}

void printStockTableSeparator() {
    std::cout << "+" << std::string(WIDTH_ID + 2, '-')
              << "+" << std::string(WIDTH_C_NAME + 2, '-')
              << "+" << std::string(WIDTH_STOCK + 2, '-')
              << "+" << std::string(WIDTH_STOCK_AVAILABILITY + 2, '-') << "+\n";
}

void printStockTableHeader() {
    printStockTableSeparator();
    std::cout << "| " << std::left << std::setw(WIDTH_ID) << "ID"
              << " | " << std::setw(WIDTH_C_NAME) << "Product Name"
              << " | " << std::right << std::setw(WIDTH_STOCK) << "Stock"
              << " | " << std::left << std::setw(WIDTH_STOCK_AVAILABILITY) << "Availability" << " |\n";
    printStockTableSeparator();
}

void printCartTableSeparator() {
    std::cout << "+" << std::string(WIDTH_ID + 2, '-')
              << "+" << std::string(WIDTH_C_NAME + 2, '-')
              << "+" << std::string(WIDTH_PRICE + 2, '-')
              << "+" << std::string(WIDTH_STOCK + 2, '-')
              << "+" << std::string(WIDTH_T_PRICE + 2, '-') << "+\n";
}

void printCartTableHeader() {
    printCartTableSeparator();
    std::cout << "| " << std::left << std::setw(WIDTH_ID) << "ID"
              << " | " << std::setw(WIDTH_C_NAME) << "Product Name"
              << " | " << std::right << std::setw(WIDTH_PRICE) << "Price"
              << " | " << std::setw(WIDTH_STOCK) << "Qty"
              << " | " << std::setw(WIDTH_T_PRICE) << "Total" << " |\n";
    printCartTableSeparator();
}

void printSalesLogSeparator(char symbol) {
    std::cout << std::string(WIDTH_REC_DATE + WIDTH_TXN_ID + WIDTH_CUSTOMER_NAME + WIDTH_ID + WIDTH_STOCK + WIDTH_T_PRICE, symbol) << "\n";
}

void printProductInfo(const ProductInfo& p) {
    std::cout << "| " << std::left << std::setw(WIDTH_ID) << p.ID
              << " | " << std::setw(WIDTH_NAME) << p.name
              << " | " << std::right << std::setw(WIDTH_PRICE) << std::fixed << std::setprecision(2) << p.price
              << " | " << std::setw(WIDTH_STOCK) << p.stockQnty << " |\n";
}

void printStockInfo(const ProductInfo& p) {
    std::string displayName = (p.name.length() > (size_t)WIDTH_NAME) ? p.name.substr(0, WIDTH_NAME-3) + "...": p.name;

    std::cout << "| " << std::left << std::setw(WIDTH_ID) << p.ID
              << " | " << std::setw(WIDTH_C_NAME) << displayName
              << " | " << std::right << std::setw(WIDTH_STOCK) << p.stockQnty
              << " | " << std::right << std::setw(WIDTH_STOCK_AVAILABILITY) << p.availability() << " |\n";
}

void printCartInfo(const CartItem& c) {
    std::string displayName = (c.name.length() > (size_t)WIDTH_C_NAME) ? c.name.substr(0, WIDTH_C_NAME-3) + "...": c.name;
    
    std::cout << "| " << std::left << std::setw(WIDTH_ID) << c.ID
              << " | " << std::setw(WIDTH_C_NAME) << displayName
              << " | " << std::right << std::setw(WIDTH_PRICE) << std::fixed << std::setprecision(2) << c.price
              << " | " << std::setw(WIDTH_STOCK) << c.orderQty
              << " | " << std::setw(WIDTH_T_PRICE) << c.total << " |\n";
}

void printReceiptHeader() {
    std::cout << " " << std::string(WIDTH_REC_DESC + WIDTH_REC_QTY + WIDTH_REC_PRICE + WIDTH_REC_TOTAL + 13 - 2, '-') << "\n"
              << "| " << std::left << std::setw(WIDTH_REC_DESC) << "Description"
              << " | " << std::setw(WIDTH_REC_QTY) << "Qty"
              << " | " << std::right << std::setw(WIDTH_REC_PRICE) << "Unit Price"
              << " | " << std::setw(WIDTH_REC_TOTAL) << "Total" << " |\n"
              << " " << std::string(WIDTH_REC_DESC + WIDTH_REC_QTY + WIDTH_REC_PRICE + WIDTH_REC_TOTAL + 13 - 2, '-') << "\n";
}

void printReceiptItem(const CartItem& c) {
    std::string displayName = c.name;
    if (displayName.length() > (size_t)WIDTH_REC_DESC) {
        displayName = displayName.substr(0, WIDTH_REC_DESC - 3) + "...";
    }
    std::cout << "| " << std::left << std::setw(WIDTH_REC_DESC) << displayName
              << " | " << std::setw(WIDTH_REC_QTY) << c.orderQty
              << " | " << std::right << std::setw(WIDTH_REC_PRICE) << std::fixed << std::setprecision(2) << c.price
              << " | " << std::setw(WIDTH_REC_TOTAL) << c.total << " |\n";
}

