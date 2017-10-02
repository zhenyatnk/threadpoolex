#pragma once

#include <threadpoolex/version/version.hpp>
#include <threadpoolex/core/TLocking.hpp>
#include <mutex>

namespace threadpoolex {
namespace core {
//--------------------------------------------------------------
template<class Type>
class TLockingEx
    :public Type, public TLockingEx<void>
{
public:
    TLockingEx() = default;

    TLockingEx(Type&& aRght)
        :Type(std::move(aRght))
    {}

    TLockingEx(const Type& aRght)
        :Type(aRght)
    {}
};

//--------------------------------------------------------------
template<>
class TLockingEx<void>
{
public:
    virtual ~TLockingEx() = default;

    virtual void force_lock()
    {
        m_ForceLocker.lock();
        m_Locker.lock();
    }
    virtual void force_unlock()
    {
        m_Locker.unlock();
        m_ForceLocker.unlock();
    }
    virtual void lock()
    {
        m_Locker.lock();
        if (!m_ForceLocker.try_lock())
        {
            m_Locker.unlock();
            std::lock_guard<std::mutex> lLock(m_ForceLocker);
            m_Locker.lock();
        }
        else
            m_ForceLocker.unlock();
    }
    virtual void unlock()
    {
        m_Locker.unlock();
    }

private:
    std::mutex m_Locker;
    std::mutex m_ForceLocker;
};
//--------------------------------------------------------------
}
}