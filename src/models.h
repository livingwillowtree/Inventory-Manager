#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>

const std::string INVENTORY_FILE_PATH = "db/inventory.txt";
const std::string SALES_FILE_PATH = "db/log.txt";

struct ProductInfo {
    std::string ID;
    std::string name;
    double price;
    int stockQnty;

    std::string availability() const {
        if (stockQnty == 0) return "[!]OUT OF STOCK";
        if (stockQnty <= 5) return "[!]Low Stock";
        return "In Stock";
    }

    bool operator==(const ProductInfo&) const = default;
};

struct CartItem {
    std::string ID;
    std::string name;
    double price;
    int orderQty;
    double total;

    bool operator==(const CartItem&) const = default;
};

struct SaleReceipt {
    std::string transactionID;
    std::string customerName;
    std::vector<CartItem> itemsBought;
    double grandTotal;
    double customerPayment;
    double change;
    std::string date;

    bool operator==(const SaleReceipt&) const = default;
};

#endif