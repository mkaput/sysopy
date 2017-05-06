NAME_ZAD1A	:= zad1a
NAME_ZAD1B	:= zad1b
NAME_ZAD1C	:= zad1c
NAME_MKF	:= mkf
SRC_ZAD1A	:= zad1a.c common.h
SRC_ZAD1B	:= zad1b.c common.h
SRC_ZAD1C	:= zad1c.c common.h
SRC_MKF		:= mkf.c common.h
OBJ_ZAD1A	:= $(SRC_ZAD1A:.c=.o)
OBJ_ZAD1B	:= $(SRC_ZAD1B:.c=.o)
OBJ_ZAD1C	:= $(SRC_ZAD1C:.c=.o)
OBJ_MKF		:= $(SRC_MKF:.c=.o)
CFLAGS	:= -Wall -std=c99 -g
LDLIBS  := -lpthread

.PHONY: all clean re

all: $(NAME_ZAD1A) $(NAME_ZAD1B) $(NAME_ZAD1C) $(NAME_MKF)

$(NAME_ZAD1A): $(OBJ_ZAD1A)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(NAME_ZAD1B): $(OBJ_ZAD1B)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(NAME_ZAD1C): $(OBJ_ZAD1C)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(NAME_MKF): $(OBJ_MKF)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	$(RM) -rf *.o *.a *.so $(NAME_ZAD1A) $(NAME_ZAD1B) $(NAME_ZAD1C) $(NAME_MKF)

re: clean all
