debug=1

TARGET=hdd_monitor
OBJECTS=main.o hdhealth.o file_operate.o config.o
CC=gcc

ifeq ($(debug), 1)
	CFLAGS=-g -Wall
else
	CFLAGS=-O2 -Wall
endif

.PHONY: all clean

all: $(TARGET)

%.o: %.c %.h
	$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ -latasmart

clean:
	rm -f $(TARGET) $(OBJECTS) *~
