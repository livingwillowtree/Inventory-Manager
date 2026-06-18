# Inventory Manager v1.0

![C++ Version](https://img.shields.io/badge/Language-C%2B%2B17-blue)

A computerized solution that can efficiently manage product information, monitor stock availability, process customer purchases, and maintain accurate inventory records.


## Key Features

* **Product CRUD Operations:** Add, update, search, and delete products.
* **Stock Monitoring:** Monitor stock availability of items. Show all items, only critically low items, or sort them ascendingly by stock quantity.
* **Order Processing and Sale Tracking:** Handles multiple items per transaction, updates stock totals in real-time, and generates multi-line itemized sales logs.
* **Inventory && Log File:** Textfiles containing the inventory and the sales log with pipe delimiting for easy external modifications and reading.
* **File Handling:** Safely reads, appends, and writes into files with a quick report of the result of the action. Success, warnings, errors.
* **Safe Exit:** Checks if the changes to the vector is reflected into the file before exiting, prompting the user if they want to exit without saving.

---

## Development Team

| Developer | Role / Module Assignment | Git Profile |
| :--- | :--- | :--- |
| **Livingwillowtree** | Code Architecture, UI Design, File Handling & Overall Optimization | [@livingwillowtree](https://github.com/livingwillowtree) |
| **Fortunity**  | Search Feature & Stock Monitoring | [@Fortunity](https://github.com/fortunity) |
| **Jaykythecoder** | Add & Update product records | [@jaykythecoder](https://github.com/jaykythecoder) |
| **Yhuriafkalways** | Menu & Submenu design & Delete Product | [@yhuriafkalways](https://github.com/yhuriafkalways) |
| **Johncarloescarilla92-cloud** | Order System & Transaction Summaries | [@johncarloescarilla92-cloud](https://github.com/johncarloescarilla92-cloud) |

---

## 

## Compilation && Execution Instructions

**⚠️ Important:** Make sure that these files and directories are present before compiling or running;
* **exe/** This will hold your executable, the program will not compile if this is not present
* **db/inventory.txt** The program will look for this file and safely quit if not found.



To build and run this system, execute the following commands inside the **root directory** in your terminal:

### **For Windows Powershell:**
```PowerShell
# Compile
g++ (Get-Item src/*.cpp) -o exe/inventoryManager.exe

# Execute the program
.\exe\inventoryManager.exe
```
### **For Windows Command Prompt (CMD):**
```DOS
# Compile
g++ src\*.cpp -o exe\inventoryManager.exe

# Execute the program
.\exe\inventoryManager.exe
```
### **For bash:**
```bash
# Compile
g++ src/*.cpp -o exe/inventoryManager

# Execute the program
exe/inventoryManager
