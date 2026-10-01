CC = gcc
CFLAGS = -Wall -Wextra -Werror

CAT_SRC = lab_1/cat/mycat.c
GREP_SRC = lab_1/grep/mygrep.c

.PHONY: all clean


all: mycat mygrep

mycat: $(CAT_SRC)
	$(CC) $(CFLAGS) $(CAT_SRC) -o mycat

mygrep: $(GREP_SRC)
	$(CC) $(CFLAGS) $(GREP_SRC) -o mygrep

clean:
	rm -f mycat mygrep
