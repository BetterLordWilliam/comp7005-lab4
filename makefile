.RECIPEPREFIX = >

CFLAGS = -Wall -Wextra -Iinclude
CC = gcc

SRC = src
BUILD = build

THREADPOOL = $(BUILD)/thread-pool
THREADPOOL_SRC = $(SRC)/thread_pool.c

COMMON_OBJ = $(BUILD)/tp_common.o
COMMON_SRC = $(SRC)/tp_common.c


all: $(THREADPOOL)


$(COMMON_OBJ): $(COMMON_SRC) | $(BUILD)
> $(CC) $(COMMON_SRC) -c -o $(COMMON_OBJ) $(CFLAGS)

$(THREADPOOL): $(THREADPOOL_SRC) $(COMMON_OBJ) | $(BUILD)
> $(CC) $(THREADPOOL_SRC) $(COMMON_OBJ) -o $(THREADPOOL) $(CFLAGS)


$(BUILD):
> mkdir -p $(BUILD)

