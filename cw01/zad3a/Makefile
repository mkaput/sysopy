NAME    := zad3a
SNAME   := $(NAME)_static
DNAME   := $(NAME)_so
LNAME   := $(NAME)_loader
SRC     := $(wildcard ../zad2/*.c)
CFLAGS	:= -Wall -std=gnu99
LDFLAGS := -L. -L../zad1/
LDLIBS  := -lzad1

.PHONY: all clean re

all : $(SNAME) $(DNAME) $(LNAME) test

$(SNAME) : LDFLAGS += -static
$(SNAME) : $(SRC)
	mkdir $@
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@/$@

$(DNAME) : CFLAGS += -Wl,-rpath=.
$(DNAME) : $(SRC)
	mkdir $@
	cp ../zad1/libzad1.so libzad1.so
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@/$@

$(LNAME) : CFLAGS += -Wl,-rpath=.
$(LNAME) : CPPFLAGS += -DDYNAMIC_TEST
$(LNAME) : LDFLAGS += -ldl
$(LNAME) : $(SRC)
	mkdir $@
	cp ../zad1/libzad1.so libzad1.so
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@/$@

test : $(LNAME)
	$(LNAME)/$(LNAME) > results.txt

clean:
	$(RM) -rf $(SNAME) $(DNAME) $(LNAME)

re: clean all
