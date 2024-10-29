# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Target executable name
TARGET = bounce

# Source files
SRC = main.c

# Object files
OBJ = $(SRC:.c=.o)

# Default target
all: $(TARGET)

# Link the program
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(TARGET) $(OBJ)

# Install the program (optional)
install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

# Uninstall the program (optional)
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Run the program with default settings
run: $(TARGET)
	./$(TARGET)

# Run with custom settings example (80x24 at 60 FPS)
run-custom: $(TARGET)
	./$(TARGET) -w 80 -h 24 -f 60

# Help target
help:
	@echo "Available targets:"
	@echo "  make        - Build the program"
	@echo "  make clean  - Remove built files"
	@echo "  make run    - Run with default settings"
	@echo "  make install   - Install to system (requires sudo)"
	@echo "  make uninstall - Remove from system (requires sudo)"
	@echo "  make run-custom - Run with 80x24 size at 60 FPS"

.PHONY: all clean install uninstall run run-custom help
