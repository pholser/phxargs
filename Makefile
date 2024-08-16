# Compiler and Linker
CC := cc

# The Target Binary Program
TARGET := pxargs

# The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR := src
BUILDDIR := build

# Flags, Libraries and Includes
CFLAGS := -g -Wall -Wextra -O2
LIB :=
INC :=

# Define the source and object files
SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SOURCES:.c=.o))

# Default make
all: directories $(BUILDDIR)/$(TARGET)

# Create the directories used in the build
directories:
	mkdir -p $(BUILDDIR)

# Link the executable
$(BUILDDIR)/$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LIB)

# Compile the source files into object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# Clean out the build directory
clean:
	$(RM) -r $(BUILDDIR)/*

# Non-file targets
.PHONY: all clean directories
