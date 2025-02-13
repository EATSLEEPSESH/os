#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

// ------------------- Аллокатор Мак-Кьюзи-Кэрелса -------------------

#define BLOCK_SIZE 64 // Размер блока памяти (в байтах)
#define MEMORY_POOL_SIZE 2048*2048*10 // Размер пула памяти
#define BITMAP_SIZE (MEMORY_POOL_SIZE / BLOCK_SIZE / 8) // Размер битовой карты

typedef struct {
    void* memory_pool; // Указатель на пул памяти
    size_t pool_size;  // Размер пула памяти
    uint8_t* bitmap;   // Битовая карта для отслеживания свободных блоков
} McKusickKarelsAllocator;

McKusickKarelsAllocator* createMcKusickKarelsAllocator(void* memory, size_t size) {
    McKusickKarelsAllocator* allocator = (McKusickKarelsAllocator*)malloc(sizeof(McKusickKarelsAllocator));
    allocator->memory_pool = memory;
    allocator->pool_size = size;
    allocator->bitmap = (uint8_t*)calloc(BITMAP_SIZE, sizeof(uint8_t)); // Инициализация битовой карты нулями
    return allocator;
}

void* mckusickKarelsAlloc(McKusickKarelsAllocator* allocator, size_t size) {
    size_t num_blocks = (size + BLOCK_SIZE - 1) / BLOCK_SIZE; // Вычисляем количество необходимых блоков

    // Поиск последовательности свободных блоков в битовой карте
    for (size_t i = 0; i < BITMAP_SIZE; ++i) {
        if (allocator->bitmap[i] != 0xFF) { // Если байт не полностью занят
            for (size_t j = 0; j < 8; ++j) {
                if (!(allocator->bitmap[i] & (1 << j))) { // Если блок свободен
                    bool found = true;
                    for (size_t k = 1; k < num_blocks; ++k) {
                        if (i + k >= BITMAP_SIZE || (allocator->bitmap[i + k] & (1 << j))) {
                            found = false;
                            break;
                        }
                    }
                    if (found) {
                        // Помечаем блоки как занятые
                        for (size_t k = 0; k < num_blocks; ++k) {
                            allocator->bitmap[i + k] |= (1 << j);
                        }
                        return (void*)((char*)allocator->memory_pool + (i * 8 + j) * BLOCK_SIZE);
                    }
                }
            }
        }
    }
    return NULL; // Нет подходящих блоков
}

void mckusickKarelsFree(McKusickKarelsAllocator* allocator, void* ptr) {
    if (!ptr) return;

    // Вычисляем индекс блока в битовой карте
    size_t block_index = ((char*)ptr - (char*)allocator->memory_pool) / BLOCK_SIZE;
    size_t byte_index = block_index / 8;
    size_t bit_index = block_index % 8;

    // Помечаем блок как свободный
    allocator->bitmap[byte_index] &= ~(1 << bit_index);
}

void destroyMcKusickKarelsAllocator(McKusickKarelsAllocator* allocator) {
    free(allocator->bitmap);
    free(allocator);
}

// ------------------- Алгоритм двойников -------------------

#define DIV_ROUNDUP(A, B) (((A) + (B)-1) / (B))
#define ALIGN_UP(A, B) (DIV_ROUNDUP((A), (B)) * (B))

typedef struct BuddyBlock {
    size_t blockSize;
    bool isFree;
} BuddyBlock;

typedef struct BuddyAllocator {
    BuddyBlock *head;
    BuddyBlock *tail;
    void *data;
    size_t totalSize;
    bool expanded;
    bool debug;
} BuddyAllocator;

BuddyBlock* next(BuddyBlock* block) {
    return (BuddyBlock*)((uint8_t*)block + block->blockSize);
}

BuddyBlock* split(BuddyBlock* block, size_t size) {
    // Пока размер блока больше нужного, делим его пополам
    while (block->blockSize > size) {
        size_t newSize = block->blockSize >> 1;
        block->blockSize = newSize;
        BuddyBlock* buddy = next(block);
        buddy->blockSize = newSize;
        buddy->isFree = true;
    }
    block->isFree = false;
    return block;
}

BuddyBlock* findBest(BuddyAllocator* allocator, size_t size) {
    BuddyBlock* block = allocator->head;
    BuddyBlock* bestBlock = NULL;

    // Ищем первый блок, который подходит по размеру
    while (block < allocator->tail) {
        if (block->isFree && block->blockSize >= size &&
            (bestBlock == NULL || block->blockSize < bestBlock->blockSize)) {
            bestBlock = block;
        }
        block = next(block);
    }

    // Если нашли подходящий блок, но его размер больше требуемого, разделяем его
    if (bestBlock) {
        // Если блок слишком велик, разделим его до нужного размера
        if (bestBlock->blockSize > size) {
            return split(bestBlock, size);
        } else {
            // Если блок идеально подходит, выделяем его
            bestBlock->isFree = false;
            return bestBlock;
        }
    }
    return NULL;
}

size_t requiredSize(size_t size) {
    size += sizeof(BuddyBlock);
    size = ALIGN_UP(size, sizeof(BuddyBlock));
    size_t actualSize = sizeof(BuddyBlock);
    while (actualSize < size) {
        actualSize <<= 1;
    }
    return actualSize;
}

void coalesce(BuddyAllocator* allocator) {
    BuddyBlock* block = allocator->head;

    // Пробегаем все блоки и сливаем соседние свободные блоки
    while (block < allocator->tail) {
        BuddyBlock* buddy = next(block);
        if (buddy >= allocator->tail) break;

        if (block->isFree && buddy->isFree && block->blockSize == buddy->blockSize) {
            block->blockSize <<= 1;
            continue;
        }
        block = next(block);
    }
}

void expand(BuddyAllocator* allocator, size_t size) {
    size_t currentSize = allocator->head ? allocator->head->blockSize : 0;
    size_t requiredSize = size + currentSize;

    // Округляем размер до ближайшей степени двойки
    size_t newSize = 1;
    while (newSize < requiredSize) {
        newSize <<= 1; // Сдвиг влево, чтобы удвоить размер
    }

    void* newData = realloc(allocator->data, newSize);
    if (!newData) {
        fprintf(stderr, "Failed to expand memory.\n");
        exit(EXIT_FAILURE);
    }

    // Обновляем указатели на новую память
    allocator->data = newData;
    allocator->head = (BuddyBlock*)allocator->data;
    allocator->tail = (BuddyBlock*)((uint8_t*)allocator->data + newSize);

    // Настраиваем блок
    allocator->head->blockSize = newSize;
    allocator->head->isFree = true;

    if (allocator->debug) {
        printf("Expanded heap to %zu bytes\n", newSize);
    }
}

BuddyAllocator* createBuddyAllocator(size_t size, bool debug) {
    BuddyAllocator* allocator = (BuddyAllocator*)malloc(sizeof(BuddyAllocator));
    allocator->data = NULL;
    allocator->head = NULL;
    allocator->tail = NULL;
    allocator->totalSize = 0;
    allocator->expanded = false;
    allocator->debug = debug;

    expand(allocator, size);
    return allocator;
}

void destroyBuddyAllocator(BuddyAllocator* allocator) {
    if (allocator->data) {
        free(allocator->data);
    }
    free(allocator);
}

void* buddyMalloc(BuddyAllocator* allocator, size_t size) {
    if (size == 0) return NULL;

    size_t actualSize = requiredSize(size);
    BuddyBlock* block = findBest(allocator, actualSize);

    if (!block) {
        if (allocator->expanded) {
            return NULL;
        }
        allocator->expanded = true;
        expand(allocator, actualSize);
        return buddyMalloc(allocator, size);
    }

    allocator->expanded = false;
    return (void*)((uint8_t*)block + sizeof(BuddyBlock));
}

void buddyFree(BuddyAllocator* allocator, void* ptr) {
    if (!ptr) return;

    BuddyBlock* block = (BuddyBlock*)((uint8_t*)ptr - sizeof(BuddyBlock));
    if ((uint8_t*)block < (uint8_t*)allocator->data ||
        (uint8_t*)block >= (uint8_t*)allocator->tail) {
        //fprintf(stderr, "Invalid pointer passed to buddyFree.\n");
        return;
    }

    block->isFree = true;

    if (allocator->debug) {
        printf("Freed %zu bytes\n", block->blockSize - sizeof(BuddyBlock));
    }

    coalesce(allocator);
}

// ------------------- Тестирование -------------------
#define MEMORY_POOL_SIZE 2048*2048*10



void testBuddyAllocator() {
    BuddyAllocator* allocator = createBuddyAllocator(MEMORY_POOL_SIZE, false);

    const int NUM_ALLOCS = 50000;
    void* blocks[NUM_ALLOCS];

    // Измеряем время выделения памяти
    clock_t start = clock();
    for (int i = 0; i < NUM_ALLOCS/2; ++i) {
        blocks[i] = buddyMalloc(allocator, 25);
        if (blocks[i] == NULL) {
            fprintf(stderr, "Failed to allocate block #%d\n", i);
        }
    }
    for (int i = NUM_ALLOCS/2; i < NUM_ALLOCS; ++i) {
        blocks[i] = buddyMalloc(allocator, 367);
        if (blocks[i] == NULL) {
            fprintf(stderr, "Failed to allocate block #%d\n", i);
        }
    }
    clock_t end = clock();
    double alloc_duration = (double)(end - start) / CLOCKS_PER_SEC;
    printf("BuddyAllocator: Time for allocations: %f seconds\n", alloc_duration);

    // Измеряем время освобождения памяти
    start = clock();
    for (int i = 0; i < NUM_ALLOCS; ++i) {
        buddyFree(allocator, blocks[i]);
    }
    end = clock();
    double free_duration = (double)(end - start) / CLOCKS_PER_SEC;
    printf("BuddyAllocator: Time for frees: %f seconds\n", free_duration);

    destroyBuddyAllocator(allocator);
}

void testMcKusickKarelsAllocator() {
    void* memory_pool = malloc(MEMORY_POOL_SIZE);
    McKusickKarelsAllocator* mckusick_karels_allocator = createMcKusickKarelsAllocator(memory_pool, MEMORY_POOL_SIZE);

    const int NUM_ALLOCS = 100000;
    void* blocks[NUM_ALLOCS];

    // Измеряем время выделения памяти
    clock_t start = clock();
    for (int i = 0; i < NUM_ALLOCS/2; ++i) {
        blocks[i] = mckusickKarelsAlloc(mckusick_karels_allocator, 25);
        if (blocks[i] == NULL) {
            fprintf(stderr, "Failed to allocate block #%d\n", i);
        }
    }
    for (int i = NUM_ALLOCS/2; i < NUM_ALLOCS; ++i) {
        blocks[i] = mckusickKarelsAlloc(mckusick_karels_allocator, 367);
        if (blocks[i] == NULL) {
            fprintf(stderr, "Failed to allocate block #%d\n", i);
        }
    }
    clock_t end = clock();
    double alloc_duration = (double)(end - start) / CLOCKS_PER_SEC;
    printf("McKusickKarelsAllocator: Time for allocations: %f seconds\n", alloc_duration);

    // Измеряем время освобождения памяти
    start = clock();
    for (int i = 0; i < NUM_ALLOCS; ++i) {
        mckusickKarelsFree(mckusick_karels_allocator, blocks[i]);
    }
    end = clock();
    double free_duration = (double)(end - start) / CLOCKS_PER_SEC;
    printf("McKusickKarelsAllocator: Time for frees: %f seconds\n", free_duration);

    destroyMcKusickKarelsAllocator(mckusick_karels_allocator);
    free(memory_pool);
}


int main() {
    printf("Starting performance tests...\n");

    // Тестирование ListAllocator
    testMcKusickKarelsAllocator();

    // Тестирование BuddyAllocator
    testBuddyAllocator();

    printf("Performance tests completed.\n");

    return 0;
}
