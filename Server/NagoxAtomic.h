#pragma once
#include "pch.h"

namespace NagoxAtomic
{
    template <typename T, std::size_t Alignment>
    concept Aligned = (alignof(T) == Alignment);

    template <typename T>
    concept AtomicType = std::is_trivial_v<T> && (sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8);

    template <typename T>
    concept BIT8 = AtomicType<T> && (sizeof(T) == 1);

    template <typename T>
    concept BIT16 = AtomicType<T> && (sizeof(T) == 2) && Aligned<T, 2>;

    template <typename T>
    concept BIT32 = AtomicType<T> && (sizeof(T) == 4) && Aligned<T, 4>;

    template <typename T>
    concept BIT64 = AtomicType<T> && (sizeof(T) == 8) && Aligned<T, 8>;

    template <typename T>
    concept PTR = std::is_pointer_v<T> && Aligned<T, 8>;


    template <AtomicType T>
    class Atomic {};

#define CONVERT_VALUE_TYPE(ConvertType, origin_) (reinterpret_cast<const ConvertType&>(const_cast<const std::remove_volatile_t<decltype(origin_)>&>(origin_)))

    template <BIT8 T>
    class Atomic<T>
    {
    public:
        Atomic(const Atomic&) = delete;
        Atomic& operator=(const Atomic&) = delete;
        Atomic(Atomic&&)noexcept = delete;
        Atomic& operator=(Atomic&&)noexcept = delete;

    public:
        inline explicit Atomic(const T initialValue = 0) noexcept
            : value{ CONVERT_VALUE_TYPE(CHAR, initialValue) } {
        }

        T load_relaxed() const noexcept {
            return CONVERT_VALUE_TYPE(T, value);
        }

        void store_relaxed(const T newValue) noexcept {
            value = CONVERT_VALUE_TYPE(CHAR, newValue);
        }

        T load_atomic() const noexcept {
            _Compiler_barrier();
            return load_relaxed();
        }

        void store_atomic(const T newValue) noexcept {
            InterlockedExchange8(&value, CONVERT_VALUE_TYPE(CHAR, newValue));
        }

        T fetch_add(const T operand) noexcept requires std::is_integral_v<T> {
            return InterlockedExchangeAdd8(&value, CONVERT_VALUE_TYPE(CHAR, operand));
        }

        T fetch_sub(const T operand) noexcept requires std::is_integral_v<T> {
            return fetch_add(-operand);
        }

        T exchange(const T newValue) noexcept {
            union Converter {
                CHAR o;
                T t;
            }; return Converter{ InterlockedExchange8(&value, CONVERT_VALUE_TYPE(CHAR, newValue)) }.t;
        }

        bool compare_exchange(const T expected, const T desired) noexcept {
            return _InterlockedCompareExchange8(&value, CONVERT_VALUE_TYPE(CHAR, desired), CONVERT_VALUE_TYPE(CHAR, expected)) ==
                CONVERT_VALUE_TYPE(CHAR, expected);
        }

    public:
        T operator++() noexcept requires std::is_integral_v<T> {
            return fetch_add(1) + 1;
        }

        T operator++(int) noexcept requires std::is_integral_v<T> {
            return fetch_add(1);
        }

        T operator--() noexcept requires std::is_integral_v<T> {
            return fetch_sub(1) - 1;
        }

        T operator--(int) noexcept requires std::is_integral_v<T> {
            return fetch_sub(1);
        }

    private:
        volatile CHAR value;
    };

    template <BIT16 T>
    class Atomic<T>
    {
    public:
        Atomic(const Atomic&) = delete;
        Atomic& operator=(const Atomic&) = delete;
        Atomic(Atomic&&)noexcept = delete;
        Atomic& operator=(Atomic&&)noexcept = delete;

    public:
        inline explicit Atomic(const T initialValue = 0) noexcept
            : value{ CONVERT_VALUE_TYPE(SHORT, initialValue) } {
        }

        T load_relaxed() const noexcept {
            return CONVERT_VALUE_TYPE_R(T, value);
        }

        void store_relaxed(const T newValue) noexcept {
            value = CONVERT_VALUE_TYPE(SHORT, newValue);
        }

        T load_atomic() const noexcept {
            _Compiler_barrier();
            return load_relaxed();
        }

        void store_atomic(const T newValue) noexcept {
            InterlockedExchange16(&value, CONVERT_VALUE_TYPE(SHORT, newValue));
        }

        T fetch_add(const T operand) noexcept requires std::is_integral_v<T> {
            return InterlockedExchangeAdd16(&value, CONVERT_VALUE_TYPE(SHORT, operand));
        }

        T fetch_sub(const T operand) noexcept requires std::is_integral_v<T> {
            return fetch_add(-operand);
        }

        T exchange(const T newValue) noexcept {
            auto temp = InterlockedExchange16(&value, CONVERT_VALUE_TYPE(SHORT, newValue));
            return reinterpret_cast<T&>(temp);
        }

        bool compare_exchange(const T expected, const T desired) noexcept {
            return InterlockedCompareExchange16(&value, CONVERT_VALUE_TYPE(SHORT, desired), CONVERT_VALUE_TYPE(SHORT, expected)) ==
                CONVERT_VALUE_TYPE(SHORT, expected);
        }

    public:
        T operator++() noexcept requires std::is_integral_v<T> {
            return InterlockedIncrement16(&value);
        }

        T operator++(int) noexcept requires std::is_integral_v<T> {
            return fetch_add(1);
        }

        T operator--() noexcept requires std::is_integral_v<T> {
            return InterlockedDecrement16(&value);
        }

        T operator--(int) noexcept requires std::is_integral_v<T> {
            return fetch_sub(1);
        }

    private:
        alignas(2) volatile SHORT value;
    };

    template <BIT32 T>
    class Atomic<T>
    {
    public:
        Atomic(const Atomic&) = delete;
        Atomic& operator=(const Atomic&) = delete;
        Atomic(Atomic&&)noexcept = delete;
        Atomic& operator=(Atomic&&)noexcept = delete;

    public:
        inline explicit Atomic(const T initialValue = 0) noexcept
            : value{ CONVERT_VALUE_TYPE(LONG, initialValue) } {
        }

        T load_relaxed() const noexcept {
            return CONVERT_VALUE_TYPE(T, value);
        }

        void store_relaxed(const T newValue) noexcept {
            value = CONVERT_VALUE_TYPE(LONG, newValue);
        }

        T load_atomic() const noexcept {
            _Compiler_barrier();
            return load_relaxed();
        }

        void store_atomic(const T newValue) noexcept {
            InterlockedExchange(&value, CONVERT_VALUE_TYPE(LONG, newValue));
        }

        T fetch_add(const T operand) noexcept {
            return InterlockedExchangeAdd(&value, CONVERT_VALUE_TYPE(LONG, operand));
        }

        T fetch_sub(const T operand) noexcept {
            return fetch_add(-operand);
        }

        T exchange(const T newValue) noexcept {
            auto temp = InterlockedExchange(&value, CONVERT_VALUE_TYPE(LONG, newValue));
            return reinterpret_cast<T&>(temp);
        }

        bool compare_exchange(const T expected, const T desired) noexcept {
            return InterlockedCompareExchange(&value, CONVERT_VALUE_TYPE(LONG, desired), CONVERT_VALUE_TYPE(LONG, expected)) ==
                CONVERT_VALUE_TYPE(LONG, expected);
        }

    public:
        T operator++() noexcept requires std::is_integral_v<T> {
            return InterlockedIncrement(&value);
        }

        T operator++(int) noexcept requires std::is_integral_v<T> {
            return fetch_add(1);
        }

        T operator--() noexcept requires std::is_integral_v<T> {
            return InterlockedDecrement(&value);
        }

        T operator--(int) noexcept requires std::is_integral_v<T> {
            return fetch_sub(1);
        }

    private:
        alignas(4) volatile LONG value;
    };

    template <BIT64 T> requires !std::is_pointer_v<T>
    class Atomic<T>
    {
    public:
        Atomic(const Atomic&) = delete;
        Atomic& operator=(const Atomic&) = delete;
        Atomic(Atomic&&)noexcept = delete;
        Atomic& operator=(Atomic&&)noexcept = delete;

    public:
        inline explicit Atomic(const T initialValue = 0) noexcept
            : value{ CONVERT_VALUE_TYPE(LONG64, initialValue) } {
        }

        T load_relaxed() const noexcept {
            return CONVERT_VALUE_TYPE(T, value);
        }

        void store_relaxed(const T newValue) noexcept {
            value = CONVERT_VALUE_TYPE(LONG64, newValue);
        }

        T load_atomic() const noexcept {
            _Compiler_barrier();
            return load_relaxed();
        }

        void store_atomic(const T newValue) noexcept {
            InterlockedExchange64(&value, CONVERT_VALUE_TYPE(LONG64, newValue));
        }

        T fetch_add(const T operand) noexcept {
            return InterlockedExchangeAdd64(&value, CONVERT_VALUE_TYPE(LONG64, operand));
        }

        T fetch_sub(const T operand) noexcept {
            return fetch_add(-operand);
        }

        T exchange(const T newValue) noexcept {
            auto temp = InterlockedExchange64(&value, CONVERT_VALUE_TYPE(LONG64, newValue));
            return reinterpret_cast<T&>(temp);
        }

        bool compare_exchange(const T expected, const T desired) noexcept {
            return InterlockedCompareExchange64(&value, CONVERT_VALUE_TYPE(LONG64, desired), CONVERT_VALUE_TYPE(LONG64, expected)) ==
                CONVERT_VALUE_TYPE(LONG64, expected);
        }

    public:
        T operator++() noexcept requires std::is_integral_v<T> {
            return InterlockedIncrement64(&value);
        }

        T operator++(int) noexcept requires std::is_integral_v<T> {
            return fetch_add(1);
        }

        T operator--() noexcept requires std::is_integral_v<T> {
            return InterlockedDecrement64(&value);
        }

        T operator--(int) noexcept requires std::is_integral_v<T> {
            return fetch_sub(1);
        }

    private:
        alignas(8) volatile LONG64 value;
    };

    template <PTR T>
    class Atomic<T>
    {
    public:
        Atomic(const Atomic&) = delete;
        Atomic& operator=(const Atomic&) = delete;
        Atomic(Atomic&&)noexcept = delete;
        Atomic& operator=(Atomic&&)noexcept = delete;

    public:
        inline explicit Atomic(const T initialValue = nullptr) noexcept
            : value{ (PVOID)(initialValue) } {
        }

        T load_relaxed() const noexcept {
            return static_cast<T>(value);
        }

        void store_relaxed(const T newValue) noexcept {
            value = (PVOID)(newValue);
        }

        T load_atomic() const noexcept {
            _Compiler_barrier();
            return load_relaxed();
        }

        void store_atomic(const T newValue) noexcept {
            InterlockedExchangePointer(&value, newValue);
        }

        T exchange(const T newValue) noexcept {
            return static_cast<T>(InterlockedExchangePointer(&value, newValue));
        }

        bool compare_exchange(const T expected, const T desired) noexcept {
            return InterlockedCompareExchangePointer(&value, desired, expected) == (PVOID)expected;
        }

    private:
        alignas(8) volatile PVOID value;
    };
}