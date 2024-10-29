# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -O2

# Release build flags with more aggressive optimizations
RELEASE_FLAGS = -O3 -march=native -flto -ffast-math -DNDEBUG

# Target executable name
TARGET = bounce
RELEASE_TARGET = $(TARGET)_release

# Source files
SRC = main.c

# Object files
OBJ = $(SRC:.c=.o)
RELEASE_OBJ = $(SRC:.c=.release.o)

# Default target
all: $(TARGET)

# Release target
release: CFLAGS += $(RELEASE_FLAGS)
release: $(RELEASE_TARGET)

# Link the debug program
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Link the release program
$(RELEASE_TARGET): $(RELEASE_OBJ)
	$(CC) $(CFLAGS) $(RELEASE_OBJ) -o $(RELEASE_TARGET)

# Compile source files for debug build
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile source files for release build
%.release.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(TARGET) $(RELEASE_TARGET) $(OBJ) $(RELEASE_OBJ)

# Install the program (optional)
install: $(RELEASE_TARGET)
	install -m 755 $(RELEASE_TARGET) /usr/local/bin/$(TARGET)

# Uninstall the program (optional)
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Run the program with default settings
run: $(TARGET)
	./$(TARGET)

# Run the release version
run-release: $(RELEASE_TARGET)
	./$(RELEASE_TARGET)

# Run with custom settings example (80x24 at 60 FPS)
run-custom: $(TARGET)
	./$(TARGET) -w 80 -h 24 -f 60

# Run release version with custom settings
run-custom-release: $(RELEASE_TARGET)
	./$(RELEASE_TARGET) -w 80 -h 24 -f 60

# Help target
help:
	@echo "Available targets:"
	@echo "  make            - Build debug version"
	@echo "  make release    - Build optimized release version"
	@echo "  make clean      - Remove built files"
	@echo "  make run        - Run debug version with default settings"
	@echo "  make run-release - Run release version with default settings"
	@echo "  make run-custom - Run debug version with 80x24 size at 60 FPS"
	@echo "  make run-custom-release - Run release version with 80x24 size at 60 FPS"
	@echo "  make install    - Install release version to system (requires sudo)"
	@echo "  make uninstall  - Remove from system (requires sudo)"

.PHONY: all clean install uninstall run run-release run-custom run-custom-release help release