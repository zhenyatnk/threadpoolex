#pragma once

#include <threadpoolex/version/version.hpp>
#include <threadpoolex/core/TNotifier.hpp>

#include <mutex>
#include <atomic>

namespace threadpoolex {
namespace core {
//--------------------------------------------------------------
template<class Type>
class TLockingShared
    :public Type, public TLockingShared<void>
{};

template<>
class TLockingShared <void>
{
public:
    explicit TLockingShared()
        :m_SharedCount(0)
    {}

    virtual ~TLockingShared() = default;

    virtual void shared_lock()
    {
        std::lock_guard<std::mutex> lLock(m_ExclusiveLock);
        m_SharedCount++;
    }
    virtual void shared_unlock()
    {
        std::lock_guard<std::mutex> lLock(m_CountLock);
        if (!!m_SharedCount && !--m_SharedCount)
            m_SharedUnlock.notify_one();
    }

    virtual void lock()
    {
        m_ExclusiveLock.lock();
        std::lock_guard<std::mutex> lLock(m_CountLock);
        if(!!m_SharedCount)
            m_SharedUnlock.wait();
    }
    virtual void unlock()
    {
        m_ExclusiveLock.unlock();
    }

private:
    std::mutex m_ExclusiveLock;
    std::mutex m_CountLock;
    std::atomic_int m_SharedCount;
    TNotifier<void> m_SharedUnlock;
};
//--------------------------------------------------------------
}
}