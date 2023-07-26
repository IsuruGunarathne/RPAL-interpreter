# Makefile for rpal_final

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17

# Source files and object files
SRCS := main.cpp
OBJS := $(SRCS:.cpp=.o)

# Header files
HDRS := Token.h TreeNode.h Tree.h TokenStorage.h Lexer.h Parser.h CSE.h

# Target executable
TARGET := rpal20

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)
	del /Q *.o

# Compiling source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Header dependencies
$(OBJS): $(HDRS)

# Clean
clean:
	del /Q *.o rpal20.exe
