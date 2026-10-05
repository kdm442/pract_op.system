CC = gcc
CFLAGS = -Wall -Wextra -Werror

CAT_SRC = cat/mycat.c
GREP_SRC = grep/mygrep.c
LS_SRC = ls/myls.c
CHMOD_SRC = chmod/mychmod.c

.PHONY: all clean

all: mycat mygrep myls mychmod

mycat: $(CAT_SRC)
	$(CC) $(CFLAGS) $(CAT_SRC) -o mycat

mygrep: $(GREP_SRC)
	$(CC) $(CFLAGS) $(GREP_SRC) -o mygrep

myls: $(LS_SRC)
	$(CC) $(CFLAGS) $(LS_SRC) -o myls

mychmod: $(CHMOD_SRC)
	$(CC) $(CFLAGS) $(CHMOD_SRC) -o mychmod

clean:
	rm -f mycat mygrep myls mychmod