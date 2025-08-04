GCC = C:/msys64/mingw64/bin/gcc.exe

BUILD = ./Build
SRC = ./engine

$(BUILD):
	mkdir -p $(BUILD)



build: $(BUILD)
	GCC -c $(SRC)/*.c -o $(BUILD)/$(SRC)*.o
	GCC -c $(SRC)/Graphics/*.c -o $(BUILD)/$(SRC)*.o
	GCC -c $(SRC)Graphics/Window/*.c -o $(BUILD)/$(SRC)*.o
	GCC -o $(BUILD)/$(SRC)/*.o
