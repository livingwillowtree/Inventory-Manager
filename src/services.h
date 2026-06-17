#ifndef SERVICES_H
#define SERVICES_H

#include <vector>
#include <string>
#include "models.h"


struct ServicesReports {
    const std::string operationSuccess  = "[Success]: Action executed successfully.\n";

    const std::string operationCancelled   = "[Notice]: Operation cancelled safely.\n";
    const std::string criticalStockNone = "[Notice]: No critical low stock items currently monitored.";

    const std::string searchUnavailable = "[Warning]: Search unavailable. Database contains no item data tracks.\n";
    const std::string outOfStock        = "[Warning]: Selection is currently out of stock.\n";
    const std::string lowAllocation     = "[Warning]: Insufficient product stock allocation for requested units.\n";
    const std::string noLogsFound       = "[Warning]: No sales logs available to display.\n";

    const std::string inventoryEmpty    = "[Inventory Error]: System inventory is entirely empty.\n";
    const std::string productNotFound   = "[Search Error]: Requested Product ID not found in database.\n";
    const std::string nameNotFound      = "[Search Error]: No records matched string entry.\n";
    const std::string cartEmpty         = "[Cart Error]: Cart is completely empty, operation rejected.\n";
}const smsg;


void displayMainMenu();
void displayInvMngMenu();
void displayStockMonMenu();
void displaySalesMenu();
void displayProducts(const std::vector<ProductInfo>& products);
void displayStock(const std::vector<ProductInfo>& products, bool criticalOnly = false);
void displayCart(const std::vector<CartItem>& cart, const double total);
void displaySalesLog(const std::vector<SaleReceipt>& salesLog);

void addProduct(std::vector<ProductInfo>& inventory);
void updateProduct(std::vector<ProductInfo>& products);
void searchProduct(const std::vector<ProductInfo>& products);
void deleteProduct(std::vector<ProductInfo>& products);

void processOrder(std::vector<ProductInfo>& products, std::vector<SaleReceipt>& salesLog);

#endif