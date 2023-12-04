# client/Makefile

BUILD_DIR = build  # 定義編譯產生的檔案存放的目錄
TARGET = $(BUILD_DIR)/client  # 定義最終目標檔案的路徑和名稱

all: $(TARGET)  # 定義預設目標，依賴於 $(TARGET)

$(TARGET): $(BUILD_DIR)/client.o | $(BUILD_DIR)  # 產生目標檔案，依賴於 client.o 和 $(BUILD_DIR) 目錄
	gcc -o $@ $^  # 編譯命令

$(BUILD_DIR)/client.o: client.c | $(BUILD_DIR)  # 產生 client.o，依賴於 client.c 和 $(BUILD_DIR) 目錄
	gcc -c $< -o $@  # 編譯命令

$(BUILD_DIR):  # 產生 $(BUILD_DIR) 目錄
	mkdir -p $@  # 建立目錄的命令

clean:  # 清除目標
	rm -rf *.o $(BUILD_DIR)  # 刪除所有 .o 檔案和 $(BUILD_DIR) 目錄

dep:  # 產生依賴檔案
	gcc -M *.c > .depend  # 產生依賴檔案的命令
	sinclude .depend  # 包含依賴檔案
