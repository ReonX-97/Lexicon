# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
LDFLAGS =

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Target executable (inside bin)
TARGET_NAME = tokenizer
TARGET = $(BINDIR)/$(TARGET_NAME)

# Source and object files
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Default target
all: $(TARGET)

# Build final executable
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compile source to object
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Force rebuild
rebuild: clean all

# Install (customize path if needed)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/$(TARGET_NAME)

# Uninstall
uninstall:
	rm -f /usr/local/bin/$(TARGET_NAME)

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

# Release build
release: CFLAGS += -O2 -DNDEBUG
release: $(TARGET)

# Help
help:
	@echo "Available targets:"
	@echo "  all       - Build the project (default)"
	@echo "  clean     - Remove build artifacts"
	@echo "  rebuild   - Clean and build"
	@echo "  debug     - Build with debug symbols"
	@echo "  release   - Build optimized release version"
	@echo "  install   - Install to /usr/local/bin/"
	@echo "  uninstall - Remove from /usr/local/bin/"
	@echo "  help      - Show this help message"

# Phony targets
.PHONY: all clean rebuild install uninstall debug release help
