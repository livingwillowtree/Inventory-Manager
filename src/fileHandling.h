#ifndef FILEHANDLING_H
#define FILEHANDLING_H

#include <vector>
#include <string>

#include "models.h"

struct FileReports {
    const std::string fileCantBeRead = "[File Error]: File cannot be read.\n";
    const std::string corruptLineSkipped = "[File Error]: Line not parsed properly.\n";
    const std::string parsedWithSkippedLines = "[File Warning]: File read successfully, but some lines are skipped.\n\n";
    const std::string newLogCreated = "[WARNING] No existing sales log found. A new one will be created upon transaction.\n\n";
    const std::string fileNotOpened = "[File Error]: File was not opened properly. Saving aborted.\n";
    const std::string savingFailed = "[System Error]: Failed to finalize the file save.\n";
    const std::string writingFailed = "[System Error]: There was an error writing into file.\n";

    const std::string successLoad = "File read successfully.\n\n";
    const std::string successSave = "File saved successfully!\n";
}const fmsg;

struct FileStatus {
    static constexpr int SUCCESS = 0;
    static constexpr int WARNING_PARSING_ERRORS = 1;
    static constexpr int ERROR_FILE_NOT_READ = 2;
    static constexpr int ERROR_FILE_NOT_SAVED = 3;
    static constexpr int ERROR_FILE_NOT_OPENED = 4;
    static constexpr int ERROR_WRITING = 5;
    static constexpr int ERROR_LOG_NOT_FOUND = 6;
} const fileStatus;

int loadInventory(std::vector<ProductInfo>& productInventory);
int saveInventory(const std::vector<ProductInfo>& productInventory);
int loadLogs(std::vector<SaleReceipt>& salesLog);
int saveLogs(const std::vector<SaleReceipt>& salesLog);

#endif