SRC = ./src/
EXEP = ./exe/
EXE = inventoryManager
inv: $(SRC)main.cpp
	g++ -Wall -Wextra -o $(EXEP)$(EXE) $(SRC)*.cpp

run: 
	$(EXEP)$(EXE)
