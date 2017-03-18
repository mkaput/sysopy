NAME	:= zad1
SRC		:= $(wildcard *.c)
OBJ		:= $(SRC:.c=.o)
CFLAGS	:= -Wall -std=c99 -g

.PHONY: all clean re

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

bench: $(NAME)
	$(RM) -f wyniki.txt

	echo "----------------------" >> wyniki.txt
	echo " 500 4" >> wyniki.txt
	echo "" >> wyniki.txt
	./$(NAME) generate test_lib.bin 500 4
	cp test_lib.bin test_sys.bin
	echo -n "shuffle sys" >> wyniki.txt
	{ time ./$(NAME) shuffle sys test_sys.bin 500 4 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "shuffle lib" >> wyniki.txt
	{ time ./$(NAME) shuffle lib test_lib.bin 500 4 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "sort sys" >> wyniki.txt
	{ time ./$(NAME) sort sys test_sys.bin 500 4 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "sort lib" >> wyniki.txt
	{ time ./$(NAME) sort lib test_lib.bin 500 4 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt

	echo "----------------------" >> wyniki.txt
	echo " 500 512" >> wyniki.txt
	echo "" >> wyniki.txt
	./$(NAME) generate test_lib.bin 500 512
	cp test_lib.bin test_sys.bin
	echo -n "shuffle sys" >> wyniki.txt
	{ time ./$(NAME) shuffle sys test_sys.bin 500 512 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "shuffle lib" >> wyniki.txt
	{ time ./$(NAME) shuffle lib test_lib.bin 500 512 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "sort sys" >> wyniki.txt
	{ time ./$(NAME) sort sys test_sys.bin 500 512 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "sort lib" >> wyniki.txt
	{ time ./$(NAME) sort lib test_lib.bin 500 512 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt

	echo "----------------------" >> wyniki.txt
	echo " 500 4096" >> wyniki.txt
	echo "" >> wyniki.txt
	./$(NAME) generate test_lib.bin 500 4096
	cp test_lib.bin test_sys.bin
	echo -n "shuffle sys" >> wyniki.txt
	{ time ./$(NAME) shuffle sys test_sys.bin 500 4096 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "shuffle lib" >> wyniki.txt
	{ time ./$(NAME) shuffle lib test_lib.bin 500 4096 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "sort sys" >> wyniki.txt
	{ time ./$(NAME) sort sys test_sys.bin 500 4096 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "sort lib" >> wyniki.txt
	{ time ./$(NAME) sort lib test_lib.bin 500 4096 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt

	echo "----------------------" >> wyniki.txt
	echo " 500 8192" >> wyniki.txt
	echo "" >> wyniki.txt
	./$(NAME) generate test_lib.bin 500 8192
	cp test_lib.bin test_sys.bin
	echo -n "shuffle sys" >> wyniki.txt
	{ time ./$(NAME) shuffle sys test_sys.bin 500 8192 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "shuffle lib" >> wyniki.txt
	{ time ./$(NAME) shuffle lib test_lib.bin 500 8192 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "sort sys" >> wyniki.txt
	{ time ./$(NAME) sort sys test_sys.bin 500 8192 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "sort lib" >> wyniki.txt
	{ time ./$(NAME) sort lib test_lib.bin 500 8192 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt

	echo "----------------------" >> wyniki.txt
	echo " 5000 4" >> wyniki.txt
	echo "" >> wyniki.txt
	./$(NAME) generate test_lib.bin 5000 4
	cp test_lib.bin test_sys.bin
	echo -n "shuffle sys" >> wyniki.txt
	{ time ./$(NAME) shuffle sys test_sys.bin 5000 4 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "shuffle lib" >> wyniki.txt
	{ time ./$(NAME) shuffle lib test_lib.bin 5000 4 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "sort sys" >> wyniki.txt
	{ time ./$(NAME) sort sys test_sys.bin 5000 4 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "sort lib" >> wyniki.txt
	{ time ./$(NAME) sort lib test_lib.bin 5000 4 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt

	echo "----------------------" >> wyniki.txt
	echo " 5000 512" >> wyniki.txt
	echo "" >> wyniki.txt
	./$(NAME) generate test_lib.bin 5000 512
	cp test_lib.bin test_sys.bin
	echo -n "shuffle sys" >> wyniki.txt
	{ time ./$(NAME) shuffle sys test_sys.bin 5000 512 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "shuffle lib" >> wyniki.txt
	{ time ./$(NAME) shuffle lib test_lib.bin 5000 512 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "sort sys" >> wyniki.txt
	{ time ./$(NAME) sort sys test_sys.bin 5000 512 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "sort lib" >> wyniki.txt
	{ time ./$(NAME) sort lib test_lib.bin 5000 512 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt

	echo "----------------------" >> wyniki.txt
	echo " 5000 4096" >> wyniki.txt
	echo "" >> wyniki.txt
	./$(NAME) generate test_lib.bin 5000 4096
	cp test_lib.bin test_sys.bin
	echo -n "shuffle sys" >> wyniki.txt
	{ time ./$(NAME) shuffle sys test_sys.bin 5000 4096 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "shuffle lib" >> wyniki.txt
	{ time ./$(NAME) shuffle lib test_lib.bin 5000 4096 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "sort sys" >> wyniki.txt
	{ time ./$(NAME) sort sys test_sys.bin 5000 4096 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "sort lib" >> wyniki.txt
	{ time ./$(NAME) sort lib test_lib.bin 5000 4096 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt

	echo "----------------------" >> wyniki.txt
	echo " 5000 8192" >> wyniki.txt
	echo "" >> wyniki.txt
	./$(NAME) generate test_lib.bin 5000 8192
	cp test_lib.bin test_sys.bin
	echo -n "shuffle sys" >> wyniki.txt
	{ time ./$(NAME) shuffle sys test_sys.bin 5000 8192 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "shuffle lib" >> wyniki.txt
	{ time ./$(NAME) shuffle lib test_lib.bin 5000 8192 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "sort sys" >> wyniki.txt
	{ time ./$(NAME) sort sys test_sys.bin 5000 8192 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt
	echo -n "sort lib" >> wyniki.txt
	{ time ./$(NAME) sort lib test_lib.bin 5000 8192 ; } 2>> wyniki.txt
	echo "" >> wyniki.txt

clean:
	$(RM) -rf *.o *.a *.so $(NAME) test_*.bin

re: clean all
