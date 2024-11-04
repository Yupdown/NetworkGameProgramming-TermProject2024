#pragma once
#include "pch.h"

template<typename T>
class AtomicMemoryPool
{
    static const inline HANDLE g_handle = GetProcessHeap();
private:
    struct alignas(8) Block
    {
        std::atomic<uint64_t> combined = 0;
    };

    struct BlockChaser
    {
        Block* const target = nullptr;
        BlockChaser* next = nullptr;
    };
    std::byte* blockStart;
    std::atomic<BlockChaser*> poolTop;
    std::atomic<uint64_t> head;
    const size_t blockSize;
    const size_t maxBlockCount;
    static constexpr uint32_t maxTagValue = (1 << 24) - 1;
private:
    static constexpr const uint64_t packPointerAndTag(const Block* const ptr, const uint32_t tag) noexcept {
        const uintptr_t ptrVal = reinterpret_cast<uintptr_t>(ptr);
        return (static_cast<const uint64_t>(ptrVal) & 0x3FFFFFFFFFF) | (static_cast<const uint64_t>(tag) << 42);
    }

    static constexpr Block* const unpackPointer(const uint64_t combined) noexcept {
        return reinterpret_cast<Block* const>(combined & 0x3FFFFFFFFFF);
    }

    static constexpr const uint32_t unpackTag(const uint64_t combined) noexcept {
        return static_cast<const uint32_t>(combined >> 42);
    }

    void initialize() noexcept
    {
        for (size_t i = 0; i < maxBlockCount - 1; ++i)
        {
            Block* const block = new (blockStart + i * blockSize) Block();
            const uint64_t nextCombined = packPointerAndTag(reinterpret_cast<Block*>(blockStart + (i + 1) * blockSize), 0);
            block->combined.store(nextCombined, std::memory_order_relaxed);
        }

        Block* const lastBlock = new (blockStart + (maxBlockCount - 1) * blockSize) Block();
        lastBlock->combined.store(packPointerAndTag(nullptr, 0), std::memory_order_relaxed);

        head.store(packPointerAndTag(reinterpret_cast<Block* const>(blockStart), 0), std::memory_order_relaxed);
    }

    __forceinline T* const allocateNewBlock()noexcept
    {
        Block* const newBlock = new(::_aligned_malloc(blockSize, std::hardware_constructive_interference_size))Block();
        BlockChaser* const newTop = new(::HeapAlloc(g_handle, NULL, sizeof(BlockChaser)))BlockChaser{ newBlock };
        BlockChaser* oldTop = poolTop.load(std::memory_order_acquire);
        while (!poolTop.compare_exchange_weak(oldTop, newTop,
            std::memory_order_release, std::memory_order_relaxed))
        {
        }
        oldTop->next = newTop;
        return reinterpret_cast<T* const>(newBlock + 1);
    }

    template <typename T>
    struct alignas(8) AlignedStorage { alignas(8) Block pad; alignas(8)std::byte data[sizeof(T)]; };
public:
    AtomicMemoryPool(const size_t count)
        : blockSize{ (sizeof(T) + sizeof(Block) + 63) & ~63 }
        , maxBlockCount{ count }
        , poolTop{ new(::HeapAlloc(g_handle, NULL, sizeof(BlockChaser)))BlockChaser()}
    {
        const size_t totalSize = blockSize * maxBlockCount;
        blockStart = static_cast<std::byte* const>(::_aligned_malloc(totalSize, std::hardware_constructive_interference_size));
        initialize();
    }

    ~AtomicMemoryPool()
    {
        BlockChaser* curBlock = poolTop.load(std::memory_order_relaxed);
        while (BlockChaser* const delBlock = curBlock)
        {
            ::_aligned_free(curBlock->target);
            curBlock = curBlock->next;
            HeapFree(g_handle, NULL, delBlock);
        }
        ::_aligned_free(blockStart);
    }

    T* const allocate() noexcept
    {
        uint64_t oldCombined = head.load(std::memory_order_relaxed);
        uint64_t newCombined;
        Block* currentBlock;
        do {
            currentBlock = unpackPointer(oldCombined);
            if (!currentBlock)
            {
                return allocateNewBlock();
            }
            const uint32_t newTag = unpackTag(oldCombined) + 1;
            Block* const nextBlock = unpackPointer(currentBlock->combined.load(std::memory_order_acquire));
            newCombined = packPointerAndTag(nextBlock, newTag);
        } while (!head.compare_exchange_weak(oldCombined, newCombined,
            std::memory_order_relaxed,
            std::memory_order_relaxed));

        return reinterpret_cast<T* const>(currentBlock + 1);
    }

    void deallocate(void* const object) noexcept
    {
        if (!object)
        {
            return;
        }
        Block* const blockPtr = reinterpret_cast<Block* const>(object) - 1;
        auto& block_combined = blockPtr->combined;
        uint64_t oldHead = head.load(std::memory_order_relaxed);
        uint64_t newCombined;
        do {
            const uint32_t newTag = unpackTag(oldHead) + 1;
            newCombined = packPointerAndTag(blockPtr, newTag);
            block_combined.store(oldHead, std::memory_order_relaxed);
        } while (!head.compare_exchange_weak(oldHead, newCombined,
            std::memory_order_release,
            std::memory_order_relaxed));
    }

    void checkAndResetIfNeeded()noexcept
    {
        const uint32_t currentTag = unpackTag(head.load(std::memory_order_relaxed));
        if (currentTag >= maxTagValue)
        {
            initialize();
        }
    }

    const bool isNeedReset()const noexcept {
        return maxTagValue - 10000 <= unpackTag(head.load(std::memory_order_relaxed));
    }
};


static constexpr const uint DEFAULT_SIZE = 256;

template <typename T>
class AtomicAllocater
{
public:
    using value_type = T;

    AtomicAllocater()noexcept {}

    template<typename U>
    AtomicAllocater(const AtomicAllocater<U>&)noexcept :memPool{ std::make_unique<AtomicMemoryPool<T>>(DEFAULT_SIZE) }
    {}

    T* const allocate(const size_t)const noexcept { return memPool->allocate(); }

    void deallocate(void* const ptr, const size_t)const noexcept { memPool->deallocate(ptr); }
private:
    const std::unique_ptr<AtomicMemoryPool<T>> memPool = std::make_unique<AtomicMemoryPool<T>>(DEFAULT_SIZE);
};

template <typename T, typename... Args>
std::shared_ptr<T> MakeShared(Args&&... args)noexcept
{
    return std::allocate_shared<T>(AtomicAllocater<T>{}, std::forward<Args>(args)...);
}
