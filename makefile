CC=gcc
CFLAGS=-Wall -Wextra -O2

RUN_SOURCES=main.c
# เปลี่ยนชื่อไฟล์จาก tests_all.c เป็น Unittest.c
TEST_SOURCES=tests/Unittest.c tests/tests_e2e.c

OBJECTS=$(RUN_SOURCES:.c=.o) $(TEST_SOURCES:.c=.o)

all: run

run: $(OBJECTS)
	$(CC) $(CFLAGS) -o run $(OBJECTS)

clean:
	rm -f *.o tests/*.o run tmp_*.csv tmp_*.txt e2e_*.csv e2e_*.txt

.PHONY: all clean