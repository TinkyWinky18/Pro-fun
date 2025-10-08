CC=gcc
CFLAGS=-Wall -Wextra -O2

RUN_SOURCES=main.c
TEST_SOURCES=tests/tests_all.c tests/tests_e2e.c   # <— เพิ่มไฟล์ e2e เข้ามาที่นี่

OBJECTS=$(RUN_SOURCES:.c=.o) $(TEST_SOURCES:.c=.o)

all: run

run: $(OBJECTS)
	$(CC) $(CFLAGS) -o run $(OBJECTS)

clean:
	rm -f *.o tests/*.o run tmp_*.csv tmp_*.txt e2e_*.csv e2e_*.txt

.PHONY: all clean
