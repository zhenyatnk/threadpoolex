#pragma once

#include <threadpoolex/version/version.hpp>
#include <threadpoolex/core/TLocking.hpp>
#include <mutex>

namespace threadpoolex {
namespace core {
//--------------------------------------------------------------
template<class Type>
class TLockingEx
    :public Type
{
public:
    virtual ~TLockingEx() = default;

    virtual void exclusive_lock()
    {
        m_ExclusiveLocker.lock();
        m_Locker.lock();
    }
    virtual void exclusive_unlock()
    {
        m_Locker.unlock();
        m_ExclusiveLocker.unlock();
    }
    virtual void lock()
    {
        m_Locker.lock();
        if (!m_ExclusiveLocker.try_lock())
        {
            m_Locker.unlock();
            std::lock_guard<std::mutex> lLock(m_ExclusiveLocker);
            m_Locker.lock();
        }
        else
            m_ExclusiveLocker.unlock();
    }
    virtual void unlock()
    {
        m_Locker.unlock();
    }

private:
    std::mutex m_Locker;
    std::mutex m_ExclusiveLocker;
};
//--------------------------------------------------------------
}
}