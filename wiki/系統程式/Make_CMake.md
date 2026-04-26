# Make 與 CMake

Make 和 CMake 是軟體建構系統，用於自動化編譯、測試和部署。Make 是經典的建構工具，CMake 則是跨平臺的生成器，能輸出各種平臺的專案檔案。

## Make

Make 由 Stuart Feldman 於 1977 年在貝爾實驗室開發，是 Unix 系統的標準建構工具。

### 基本概念

Makefile 由一系列規則組成：

```
目標: 依賴
	命令
```

目標（target）：要生成的檔案或執行的動作
依賴（prerequisites）：生成目標所需的檔案
命令（recipe）：生成目標的指令

### 簡單 Makefile

```makefile
# 這是註釋
CC = gcc
CFLAGS = -Wall -g
TARGET = program
OBJS = main.o utils.o

# 隱含規則：從 .c 編譯為 .o
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c utils.h
	$(CC) $(CFLAGS) -c main.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean  # 聲明非檔案目標
```

### 自動化變數

```makefile
# 自動化變數
# $@ - 目標名
# $< - 第一個依賴
# $^ - 所有依賴
# $? - 比目標新的依賴

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# 等價於
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
```

### 模式規則

```makefile
# 模式規則：所有 .c 編譯為 .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 同時處理多個模式
%.exe: %.c
	$(CC) $(CFLAGS) $< -o $@
```

### 副函式

```makefile
# 副函式
# $(wildcard pattern) - 展開符合模式的檔案
SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:.c=.o)

# $(patsubst pattern,replacement,text) - 模式替換
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))

# $(foreach var,list,text) - 迭代
DIRS := src lib test
ALL_BINS := $(foreach dir,$(DIRS),$(dir)/binary)
```

### 條件與函式

```makefile
# 條件判斷
DEBUG = true

ifeq ($(DEBUG),true)
    CFLAGS += -g -DDEBUG
else
    CFLAGS += -O2
endif

# ifdef / ifndef
ifdef VERBOSE
    Q :=
else
    Q := @
endif

# 常用函式
$(shell ls *.c)           # 執行 shell 命令
$(subst from,to,text)      # 替換文字
$(filter pattern,text)     # 過濾
$(filter-out pattern,text) # 反向過濾
$(dir names)               # 取得目錄
$(notdir names)            # 取得檔名
$(abspath names)           # 絕對路徑
```

### 多目標建構

```makefile
# 多目標
PDFS := $(wildcard *.md)
HTMLS := $(PDFS:.md=.html)

$(HTMLS): %.html: %.md
	pandoc $< -o $@

# 即時展開所有 HTML
all: $(HTMLS)
```

### 包含其他 Makefile

```makefile
# 包含設定檔
include config.mk

# 條件包含
-include user.mk

# 如果檔案不存在，不會錯誤
```

### 遞迴 Make

```makefile
# 遞迴建構子目錄
SUBDIRS := src lib test

all:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
```

## CMake

CMake 是 Kitware 開發的跨平臺建構系統，2000 年發布。CMake 不是直接編譯，而是生成平臺原生建構系統（Make、Ninja、Visual Studio、Xcode 等）的輸入檔案。

### 基本專案結構

```
project/
├── CMakeLists.txt
├── src/
│   ├── CMakeLists.txt
│   └── main.cpp
└── include/
    └── utils.h
```

### 根目錄 CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)

# 專案名稱和語言
project(MyApp VERSION 1.0.0 LANGUAGES CXX)

# C++ 標準
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 建構類型選項
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "${default_build_type}")
endif()

# 設定輸出目錄
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 原始碼目錄
add_subdirectory(src)

# 安裝規則
install(TARGETS MyApp DESTINATION bin)
install(FILES include/utils.h DESTINATION include)
```

### 子目錄 CMakeLists.txt

```cmake
# 設定標頭檔搜尋路徑
include_directories(${PROJECT_SOURCE_DIR}/include)

# 設定原始碼
set(SOURCES
    main.cpp
    utils.cpp
    config.cpp
)

# 建立執行檔
add_executable(MyApp ${SOURCES})

# 連結函式庫
target_link_libraries(MyApp PRIVATE
    pthread
    MathUtils
)

# 設定編譯選項
target_compile_options(MyApp PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# 設定預處理器定義
target_compile_definitions(MyApp PRIVATE
    DEBUG=1
    VERSION="${PROJECT_VERSION}"
)

# 匯出標頭檔
target_include_directories(MyApp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)
```

### 函式庫

```cmake
# 靜態函式庫
add_library(MathUtils STATIC math.cpp)

# 共享函式庫
add_library(SharedLib SHARED shared.cpp)

# 物件檔案庫（無需連結）
add_library(MathObjects OBJECT math.cpp)
add_library(MathUtils STATIC $<TARGET_OBJECTS:MathObjects>)

# 條件建構函式庫
option(BUILD_SHARED "Build shared libraries" ON)
add_library(MathUtils ${BUILD_SHARED} math.cpp)
```

### 查找套件

```cmake
# 查找套件
find_package(OpenSSL REQUIRED)
find_package(ZLIB)
find_package(Threads REQUIRED)

# 使用找到的套件
target_link_libraries(MyApp
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
    Threads::Threads
)

# 查找自訂套件
find_library(MYLIB mylib PATHS /usr/local/lib)
if(MYLIB)
    target_link_libraries(MyApp ${MYLIB})
endif()
```

### 測試整合

```cmake
enable_testing()

add_executable(test_math test_math.cpp)
add_test(NAME MathTests COMMAND test_math)

# 自訂測試
add_test(NAME StressTest COMMAND ./test_stress --iterations 10000)
set_tests_properties(StressTest PROPERTIES TIMEOUT 60)
```

### 安裝與打包

```cmake
# 安裝規則
install(TARGETS MyApp MathUtils
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES
    ${CMAKE_SOURCE_DIR}/include/config.h
    DESTINATION include
)

# CPack 配置
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
include(CPack)
```

### 選項與快取

```cmake
# 選項
option(ENABLE_TESTS "Enable test suite" ON)
option(USE_OPENMP "Use OpenMP for parallelism" OFF)

# 數值選項
set(BUFFER_SIZE 4096 CACHE STRING "Buffer size")

# 敘述性選項
cmake_dependent_option(ENABLE_GUI "Build GUI" ON "GTK3_FOUND" OFF)
```

### 產生器表示式

```cmake
# 產生器表示式根據建構設定動態生成
target_include_directories(MyApp PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 設定除錯/發布專用選項
target_compile_definitions(MyApp PRIVATE
    $<$<CONFIG:Debug>:DEBUG_MODE>
    $<$<CONFIG:Release>:NDEBUG>
)

# 條件連結
target_link_libraries(MyApp PRIVATE
    $<$<BOOL:${ENABLE_OPENSSL}>:OpenSSL::SSL>
)
```

### 交叉編譯

```cmake
# toolchain.cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabi-g++)
set(CMAKE_FIND_ROOT_PATH /opt/arm-rootfs)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

使用交叉編譯：
```bash
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake ..
make
```

### 現代 CMake 實踐

```cmake
cmake_minimum_required(VERSION 3.18)

project(MyApp)

# 使用 target_* 命令（現代風格）
add_executable(myapp src/main.cpp)

target_compile_features(myapp PRIVATE cxx_std_20)
target_include_directories(myapp PRIVATE include)
target_link_libraries(myapp PRIVATE spdlog::spdlog)

# 匯出目標
install(TARGETS myapp EXPORT MyAppTargets)
install(EXPORT MyAppTargets FILE MyAppTargets.cmake DESTINATION lib/cmake)
export(EXPORT MyAppTargets FILE MyAppTargets.cmake)
```

## Make 與 CMake 的選擇

| 特性 | Make | CMake |
|------|------|-------|
| 學習曲線 | 低 | 中高 |
| 跨平臺 | 需要多個 Makefile | 單一 CMakeLists.txt |
| 大型專案支援 | 一般 | 優秀 |
| IDE 整合 | 一般 | 優秀 |
| 增量建構 | 是 | 是 |
| 隱含規則 | 豐富 | 無 |

## 相關主題

- [C語言](C語言.md) - Make 的主要應用語言
- [版本控制](版本控制.md) - 建構系統的配合使用
- [持續整合](持續整合.md) - CI/CD 中的建構自動化
