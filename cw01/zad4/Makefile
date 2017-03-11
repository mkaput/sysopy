NAME    := zad4
SRC     := $(wildcard *.c)
CFLAGS	:= -Wall -std=gnu99 -g
LDFLAGS := -L. -L../zad1/
LDLIBS  := -lzad1

.PHONY: all clean re

all : $(NAME)

$(NAME) : LDFLAGS += -static
$(NAME) : $(SRC)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	$(RM) -rf $(NAME)

re: clean all
