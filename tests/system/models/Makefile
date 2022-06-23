CC = gcc
CCDEBUG = $(CC) -g
CFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Wfatal-errors -fPIC

SRCDIR = src
OBJDIR = obj

SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o) 
OBJDEBUG = $(SRC:$(SRCDIR)/%.cpp=$(OBJDIR)/%_debug.o) 

INCPATH = -Iinclude -I../include -I../../grafed/include\
	  -I../../jsonparser/include -I../../csl/include


all: $(OBJ) $(OBJDEBUG)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(INCPATH) -c $^ -o $@ 
$(OBJDIR)/%_debug.o: $(SRCDIR)/%.cpp
	$(CCDEBUG) $(CFLAGS) $(INCPATH) -c $^ -o $@ 

.PHONY: clean
clean:
	rm $(OBJDIR)/*.o
