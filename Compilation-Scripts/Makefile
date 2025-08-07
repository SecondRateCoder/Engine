# GCC = C:/msys64/mingw64/bin/gcc.exe
GCC = gcc

BUILD = ./Build
SRC = ./engine

SRCS := $(wildcard $(SRC)/*.c) \
		$(wildcard $(SRC)/Graphics/*.c) \
		$(wildcard $(SRC)/Graphics/Window/*.c)
OBJS := $(patsubst $(SRC)/%.c,$(BUILD)/%.o,$(SRCS))

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	$(GCC) -c $< -o $@

build: $(OBJS)
	$(GCC) $(OBJS) -o $(BUILD)/engine

clean:
	rm -rf $(BUILD)

always: build
	