# client/Makefile

BUILD_DIR = build
TARGET = $(BUILD_DIR)/client

all: $(TARGET)

$(TARGET): $(BUILD_DIR)/client.o | $(BUILD_DIR)
	gcc -o $@ $^

$(BUILD_DIR)/client.o: client.c | $(BUILD_DIR)
	gcc -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf *.o $(BUILD_DIR)

dep:
	gcc -M *.c > .depend
	sinclude .depend
