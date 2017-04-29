NAME	:= zad1
SRC		:= $(wildcard *.c)
OBJ		:= $(SRC:.c=.o)
CFLAGS	:= -Wall -std=c99 -g
LDLIBS  := -lrt

.PHONY: all clean re

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	$(RM) -rf *.o *.a *.so $(NAME)

re: clean all
