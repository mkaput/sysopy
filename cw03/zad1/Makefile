NAME	:= zad1
SRC		:= main.c
OBJ		:= $(SRC:.c=.o)
CFLAGS	:= -Wall -std=c99 -g

DUMP_NAME	:= zad1_dump
DUMP_SRC	:= dump.c
DUMP_OBJ	:= $(DUMP_SRC:.c=.o)

.PHONY: all clean re test

all: $(DUMP_NAME) $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(DUMP_NAME): $(DUMP_OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

test: $(DUMP_NAME) $(NAME)
	./$(NAME) test_script.janush

clean:
	$(RM) -rf *.o *.a *.so $(DUMP_NAME) $(NAME)

re: clean all
