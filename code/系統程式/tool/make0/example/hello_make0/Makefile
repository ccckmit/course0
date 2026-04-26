CC      = gcc
CFLAGS  = -Wall -O2
TARGET  = hello
SRC     = hello.c

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<
	@echo "Built $@ successfully!"

run: $(TARGET)
	@echo "--- Running $(TARGET) ---"
	./$(TARGET)

clean:
	-rm -f $(TARGET)
	@echo "Clean done."