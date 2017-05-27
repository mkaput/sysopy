NAME_SERWER	:= serwer
NAME_KLIENT	:= klient
SRC_SERWER	:= serwer.c task.h
SRC_KLIENT	:= klient.c task.h
OBJ_SERWER	:= $(SRC_SERWER:.c=.o)
OBJ_KLIENT	:= $(SRC_KLIENT:.c=.o)
CFLAGS	:= -Wall -std=c99 -g
LDLIBS  := -lpthread

.PHONY: all clean re

all: $(NAME_SERWER) $(NAME_KLIENT)

$(NAME_SERWER): $(OBJ_SERWER)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(NAME_KLIENT): $(OBJ_KLIENT)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	$(RM) -rf *.o *.a *.so $(NAME_SERWER) $(NAME_KLIENT)

re: clean all
