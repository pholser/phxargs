CC := cc

TARGET := phxargs
LIBTARGET := libphxargs.a

SRCDIR := src
BUILDDIR := build
TESTDIR := tests

CFLAGS := -g -Wall -Wextra -O0
LIB :=
INC :=

SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SOURCES:.c=.o))

TEST_SOURCES := $(wildcard $(TESTDIR)/*.c)
TEST_OBJECTS := $(patsubst $(TESTDIR)/%, $(BUILDDIR)/%, $(TEST_SOURCES:.c=.o))
TEST_EXECUTABLES := $(patsubst $(TESTDIR)/%, $(BUILDDIR)/%, $(TEST_SOURCES:.c=))

all: directories $(BUILDDIR)/$(TARGET)

directories:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(BUILDDIR)/$(LIBTARGET): $(OBJECTS)
	ar rcs $@ $^

$(BUILDDIR)/%.o: $(TESTDIR)/%.c
	$(CC) $(CFLAGS) $(INC) -I$(SRCDIR) -c $< -o $@

$(BUILDDIR)/%: $(BUILDDIR)/%.o $(BUILDDIR)/$(LIBTARGET)
	$(CC) $^ -o $@ $(LIB) -lcheck

test: $(TEST_EXECUTABLES)
	@for script in $(TESTDIR)/test-*.sh ; do bash $$script ; done
	@for test_exec in $(TEST_EXECUTABLES); do ./$$test_exec ; done

clean:
	$(RM) -r $(BUILDDIR)/*
	$(RM) $(TESTDIR)/*.err $(TESTDIR)/*.out

.PHONY: all clean test directories
