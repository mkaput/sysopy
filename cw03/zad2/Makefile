NAME	:= zad2
SRC		:= main.c
OBJ		:= $(SRC:.c=.o)
CFLAGS	:= -Wall -std=c99 -g

DUMP_NAME	:= zad1_dump
DUMP_SRC	:= ../zad1/dump.c
DUMP_OBJ	:= $(DUMP_SRC:.c=.o)

BADBOY_NAME	:= zad2_badboy
BADBOY_SRC	:= badboy.c
BADBOY_OBJ	:= $(BADBOY_SRC:.c=.o)

.PHONY: all clean re test

all: $(DUMP_NAME) $(BADBOY_NAME) $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(DUMP_NAME): $(DUMP_OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BADBOY_NAME): $(BADBOY_OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

test: $(DUMP_NAME) $(BADBOY_NAME) $(NAME)
	./$(NAME) test_script2.janush 5 100

clean:
	$(RM) -rf *.o *.a *.so $(DUMP_NAME) $(BADBOY_NAME) $(NAME)

re: clean all
