NAME = main
SRC_NAME = $(NAME).cpp
EXE_NAME = $(NAME).x

INCLUDE_PATH = -I../../include
LIB_PATH     = -L../../lib

CC     = gcc 
CFLAGS = -std=c++17 $(INCLUDE_PATH) $(LIB_PATH)
LIBS   = -lcsl -lgsl -lgslcblas -lstdc++ -lm

$(EXE_NAME): $(SRC_NAME)
	$(CC) $(CFLAGS) -o $(EXE_NAME) $(SRC_NAME) $(LIBS)
clean:
	rm $(EXE_NAME)
