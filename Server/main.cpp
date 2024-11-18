#include "pch.h"
#include "IOExecutor.h"
#include "PacketBase.hpp"
#include "MCWorld.h"

#include <concepts>
#include <type_traits>
#include <cstdint>
#include <windows.h>
#include <concepts>
#include <type_traits>
#include <cstdint>
#include <windows.h>

template <typename T>
concept AtomicType = std::is_trivial_v<T> && (sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8);

template <typename T>
concept BIT8 = AtomicType<T> && (sizeof(T) == 1);

template <typename T>
concept BIT16 = AtomicType<T> && (sizeof(T) == 2);

template <typename T>
concept BIT32 = AtomicType<T> && (sizeof(T) == 4);

template <typename T>
concept BIT64 = AtomicType<T> && (sizeof(T) == 8);

template <typename T>
concept PTR = std::is_pointer_v<T>;  


template <AtomicType T>
class Atomic {};

template <BIT8 T>
class Atomic<T> {
public:
    Atomic(const Atomic&) = delete;
    Atomic& operator=(const Atomic&) = delete;
    Atomic(Atomic&&) = delete;
    Atomic& operator=(Atomic&&) = delete;
    explicit Atomic(const T initialValue = 0) noexcept : value(*reinterpret_cast<const CHAR*>(&initialValue)) {}
public:
    T load() const noexcept {
        return static_cast<T>(value);
    }

    void store(const T newValue) noexcept {
        value = static_cast<char>(newValue);
    }

    T fetch_add(const T operand) noexcept requires (!std::is_same_v<T, bool>) {
        return static_cast<T>(InterlockedExchangeAdd8(&value, static_cast<CHAR>(operand)));
    }

    T fetch_sub(const T operand) noexcept requires (!std::is_same_v<T, bool>) {
        return fetch_add(-operand);
    }

    T exchange(const T newValue) noexcept {
        return static_cast<T>(_InterlockedExchange8(&value, static_cast<CHAR>(newValue)));
    }

    bool compare_exchange(const T expected, const T desired) noexcept {
        return _InterlockedCompareExchange8(&value, static_cast<CHAR>(desired), static_cast<CHAR>(expected)) == static_cast<CHAR>(expected);
    }
private:
    volatile CHAR value;
};

template <BIT16 T>
class Atomic<T> {
private:
    volatile SHORT value;

public:
    explicit Atomic(const T initialValue = 0) noexcept : value(*reinterpret_cast<SHORT*>(&initialValue)) {}

    Atomic(const Atomic&) = delete;
    Atomic& operator=(const Atomic&) = delete;
    Atomic(Atomic&&) = delete;
    Atomic& operator=(Atomic&&) = delete;

    T load() const noexcept {
        return static_cast<T>(value);
    }

    void store(T newValue) noexcept {
        value = static_cast<short>(newValue);
    }

    T fetch_add(T operand) noexcept {
        return static_cast<T>(InterlockedExchangeAdd16(&value, static_cast<short>(operand)));
    }

    T fetch_sub(T operand) noexcept {
        return fetch_add(-operand);
    }

    T exchange(T newValue) noexcept {
        return static_cast<T>(_InterlockedExchange16(&value, static_cast<short>(newValue)));
    }

    bool compare_exchange(T expected, T desired) noexcept {
        return InterlockedCompareExchange16(&value, static_cast<short>(desired), static_cast<short>(expected)) == static_cast<short>(expected);
    }
};

template <BIT32 T>
class Atomic<T> {
private:
    volatile LONG value;

public:
    explicit Atomic(T initialValue = 0) noexcept : value(static_cast<long>(initialValue)) {}

    Atomic(const Atomic&) = delete;
    Atomic& operator=(const Atomic&) = delete;
    Atomic(Atomic&&) = delete;
    Atomic& operator=(Atomic&&) = delete;

    T load() const noexcept {
        return static_cast<T>(value);
    }

    void store(T newValue) noexcept {
        value = static_cast<long>(newValue);
    }

    T fetch_add(T operand) noexcept {
        return static_cast<T>(InterlockedExchangeAdd(&value, static_cast<long>(operand)));
    }

    T fetch_sub(T operand) noexcept {
        return fetch_add(-operand);
    }

    T exchange(T newValue) noexcept {
        return static_cast<T>(_InterlockedExchange(&value, static_cast<long>(newValue)));
    }

    bool compare_exchange(T expected, T desired) noexcept {
        return _InterlockedCompareExchange(&value, static_cast<long>(desired), static_cast<long>(expected)) == static_cast<long>(expected);
    }
};

// 특수화: 64비트 자료형
template <BIT64 T>requires !std::is_pointer_v<T>
class Atomic<T> {
private:
    volatile LONG64 value;

public:
    explicit Atomic(T initialValue = 0) noexcept : value(static_cast<__int64>(initialValue)) {}

    Atomic(const Atomic&) = delete;
    Atomic& operator=(const Atomic&) = delete;
    Atomic(Atomic&&) = delete;
    Atomic& operator=(Atomic&&) = delete;

    T load() const noexcept {
        return static_cast<T>(value);
    }

    void store(T newValue) noexcept {
        value = static_cast<__int64>(newValue);
    }

    T fetch_add(T operand) noexcept {
        return static_cast<T>(InterlockedExchangeAdd64(&value, static_cast<__int64>(operand)));
    }

    T fetch_sub(T operand) noexcept {
        return fetch_add(-operand);
    }

    T exchange(T newValue) noexcept {
        return static_cast<T>(InterlockedExchange64(&value, static_cast<__int64>(newValue)));
    }

    bool compare_exchange(T expected, T desired) noexcept {
        return _InterlockedCompareExchange64(&value, static_cast<__int64>(desired), static_cast<__int64>(expected)) == static_cast<__int64>(expected);
    }
};

template <PTR T>
class Atomic<T> {
private:
    volatile PVOID value;

public:
    explicit Atomic(const T initialValue = nullptr) noexcept : value(initialValue) {}

    Atomic(const Atomic&) = delete;
    Atomic& operator=(const Atomic&) = delete;
    Atomic(Atomic&&) = delete;
    Atomic& operator=(Atomic&&) = delete;

    T load() const noexcept {
        return (T)value;
    }

    void store(const T newValue) noexcept {
        value = (PVOID)newValue;
    }

    T exchange(T newValue) noexcept {
        return reinterpret_cast<T>(_InterlockedExchangePointer(&value, newValue));
    }

    bool compare_exchange(const T expected, const T desired) noexcept {
        return _InterlockedCompareExchangePointer(&value, desired, expected) == (PVOID)expected;
    }
};


int main()
{
    Atomic<char> a;
    a.load();
    a.compare_exchange(0, 0);
    if (false == Mgr(IOExecutor)->InitServer("8888"))
    {
        std::cout << "서버 초기화 실패" << std::endl;
        return 1;
    }
    
    Mgr(MCWorld)->Init();


    Mgr(IOExecutor)->IORoutine(); 
}