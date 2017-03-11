NAME    := zad1
SNAME   := lib$(NAME).a
DNAME   := lib$(NAME).so
SRC     :=  $(wildcard *.c)
OBJ     :=  $(SRC:.c=.o)
CFLAGS	:= -Wall -std=c99 -fPIC

.PHONY: all clean re

all: $(SNAME) $(DNAME)

$(SNAME): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

$(DNAME): LDFLAGS += -shared
$(DNAME): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	$(RM) -rf *.o *.a *.so

re: clean all
