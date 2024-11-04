#pragma once
#include "pch.h"

class SRWLock
{
public:
    SRWLock()noexcept { InitializeSRWLock(&SRW_lock); }
    void lock() noexcept { AcquireSRWLockExclusive(&SRW_lock); }
    void unlock()noexcept { ReleaseSRWLockExclusive(&SRW_lock); }
    void lock_shared()noexcept { AcquireSRWLockShared(&SRW_lock); }
    void unlock_shared()noexcept { ReleaseSRWLockShared(&SRW_lock); }
    const bool try_lock() noexcept { return FALSE != TryAcquireSRWLockExclusive(&SRW_lock); }
    const bool try_lock_shared()noexcept { return FALSE != TryAcquireSRWLockShared(&SRW_lock); }
private:
    SRWLOCK SRW_lock;
};