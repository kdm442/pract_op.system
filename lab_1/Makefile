CC = gcc
CFLAGS = -Wall -Wextra -Werror

CAT_SRC = lab_1/cat/mycat.c
GREP_SRC = lab_1/grep/mygrep.c
LS_SRC = lab_1/ls/myls.c

.PHONY: all clean


all: mycat mygrep myls

mycat: $(CAT_SRC)
	$(CC) $(CFLAGS) $(CAT_SRC) -o mycat

mygrep: $(GREP_SRC)
	$(CC) $(CFLAGS) $(GREP_SRC) -o mygrep

myls: $(LS_SRC_SRC)
	$(CC) $(CFLAGS) $(LS_SRC_SRC) -o myls

myls: $(CHMOD_SRC_SRC)
	$(CC) $(CFLAGS) $(CHMOD_SRC_SRC) -o chmod

clean:
	rm -f mycat mygrep myls chmod
