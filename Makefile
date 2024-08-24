# 编译器设置
CC = g++
CFLAGS_DEBUG = -Iinclude -Wall -g -std=c++20 -fdiagnostics-color=always
CFLAGS_RELEASE = -Iinclude -Wall -O3 -std=c++20 -fdiagnostics-color=always

# 默认构建类型
BUILD_TYPE ?= debug

# 选择编译选项和目录
ifeq ($(BUILD_TYPE),release)
    CFLAGS = $(CFLAGS_RELEASE)
    BUILD_DIR = build/release
    BIN_DIR = bin/release
else
    CFLAGS = $(CFLAGS_DEBUG)
    BUILD_DIR = build/debug
    BIN_DIR = bin/debug
endif

# 日志文件
ERROR_LOG = $(BUILD_DIR)/error.log

# 源文件和对象文件
SRCS = $(wildcard src/**/**/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/*.cpp)
OBJS = $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
TARGET = $(BIN_DIR)/buildTest

# 默认目标
all: cleanlog $(TARGET)

cleanlog:
	@echo "Cleaning up previous logs..."
	@rm -f $(ERROR_LOG)

# 链接
$(TARGET): $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $(TARGET) 2>> $(ERROR_LOG)

# 编译规则
$(BUILD_DIR)/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ 2>> $(ERROR_LOG)

# 清理
clean:
	rm -rf build bin

# 打开日志
see:
	vim $(ERROR_LOG)

# 调试构建
debug: BUILD_TYPE = debug
debug: all

# 发布构建
release: BUILD_TYPE = release
release: all

rebuild: clean all

.PHONY: all clean debug release see rebuild
