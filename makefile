# the compiler: gcc for C program, define as g++ for C++
CC = clang++ 
CC = g++ 

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS = -Wall -O3 -std=c++20 -fanalyzer
CFLAGS = -Wall -O3 -std=c++20

# the build target executable:
TARGET = find_longest_word_tree

all: $(TARGET)

$(TARGET): $(TARGET).cpp makefile
	$(CC) $(CFLAGS) -O3 -o $(TARGET) $(TARGET).cpp
	strip --strip-unneeded $(TARGET)

clean:
	$(RM) $(TARGET)
