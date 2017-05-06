NAME_ZAD2A	:= zad2a
NAME_ZAD2B	:= zad2b
SRC_ZAD2A	:= zad2a.c common.h
SRC_ZAD2B	:= zad2b.c
OBJ_ZAD2A	:= $(SRC_ZAD2A:.c=.o)
OBJ_ZAD2B	:= $(SRC_ZAD2B:.c=.o)
CFLAGS	:= -Wall -std=c99 -g
LDLIBS  := -lpthread

.PHONY: all clean re

all: $(NAME_ZAD2A) $(NAME_ZAD2B)

$(NAME_ZAD2A): $(OBJ_ZAD2A)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(NAME_ZAD2B): $(OBJ_ZAD2B)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	$(RM) -rf *.o *.a *.so $(NAME_ZAD2A) $(NAME_ZAD2B)

re: clean all
