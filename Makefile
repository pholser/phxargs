CC := cc

TARGET := phxargs

SRCDIR := src
BUILDDIR := build
TESTDIR := tests

CFLAGS := -g -Wall -Wextra -O2
LIB :=
INC :=

SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SOURCES:.c=.o))

all: directories $(BUILDDIR)/$(TARGET)

directories:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

test:
	@for script in $(TESTDIR)/test_*.sh ; do bash $$script ; done

clean:
	$(RM) -r $(BUILDDIR)/*
	$(RM) $(TESTDIR)/*.err $(TESTDIR)/*.out

.PHONY: all clean test directories
