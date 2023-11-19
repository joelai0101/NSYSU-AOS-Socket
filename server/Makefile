# server/Makefile

BUILD_DIR = build
TARGET = $(BUILD_DIR)/server

all: $(TARGET)

$(TARGET): $(BUILD_DIR)/server.o | $(BUILD_DIR)
	gcc -o $@ $^ -pthread

$(BUILD_DIR)/server.o: server.c | $(BUILD_DIR)
	gcc -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf *.o $(BUILD_DIR)

dep:
	gcc -M *.c > .depend
	sinclude .depend