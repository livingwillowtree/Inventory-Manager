SRC_DIR = src
EXE_DIR = exe

ifeq ($(OS),Windows_NT)
    EXE = inventoryManager.exe
    INIT_DIR = if not exist $(EXE_DIR) mkdir $(EXE_DIR)
    RUN_CMD = .\\$(EXE_DIR)\\$(EXE)
else
    EXE = inventoryManager
    INIT_DIR = mkdir -p $(EXE_DIR)
    RUN_CMD = ./$(EXE_DIR)/$(EXE)
endif

TARGET = $(EXE_DIR)/$(EXE)

SRCS = $(wildcard $(SRC_DIR)/*.cpp)

all: $(TARGET)

$(TARGET): $(SRCS)
	@$(INIT_DIR)
	g++ -Wall -Wextra -o $(TARGET) $(SRCS)

run: $(TARGET)
	$(RUN_CMD)

.PHONY: all run