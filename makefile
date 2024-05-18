# Define compiler
CC = gcc

# Define C compiler flags (feel free to customize)
CFLAGS = -Wall -g

# Define include directory path (assuming makefile is in top directory)
IDIR = ./include

# Define source directory path
SRCDIR = ./src

# Define object files
OBJS = $(SRCDIR)/*.o

# Define target executable name
TARGET = main

# Define all rule (builds the final executable)
all: $(TARGET)

# Define rule to build the executable 
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) ./src/main.c $(OBJS) -L$(IDIR) -lm

# Define rule to build object files from source files
$(SRCDIR)/%.o: $(SRCDIR)/%.c $(IDIR)/%.h
	$(CC) $(CFLAGS) -c -I$(IDIR) $< -o $@

# Phony clean target to remove object files and executable
clean:
	rm -f $(OBJS) $(TARGET)
