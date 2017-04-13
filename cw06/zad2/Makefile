NAME_SERWER	:= serwer
NAME_KLIENT	:= klient
SRC_SERWER	:= serwer.c
SRC_KLIENT	:= klient.c
OBJ_SERWER	:= $(SRC_SERWER:.c=.o)
OBJ_KLIENT	:= $(SRC_KLIENT:.c=.o)
CFLAGS		:= -Wall -std=c99 -g
LDFLAGS		:= -lrt

.PHONY: all clean re

all: $(NAME_SERWER) $(NAME_KLIENT)

$(NAME_SERWER): $(OBJ_SERWER)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(NAME_KLIENT): $(OBJ_KLIENT)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	$(RM) -rf *.o *.a *.so $(NAME_SERWER) $(NAME_KLIENT)

re: clean all
