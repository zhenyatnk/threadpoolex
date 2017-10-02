#pragma once

#include <threadpoolex/version/version.hpp>
#include <mutex>
#include <utility>

namespace threadpoolex {
namespace core {
//--------------------------------------------------------------
template<class Type>
class TLocking
    :public Type, public TLocking<void>
{
public:
    TLocking() = default;

    TLocking(const Type& aRght)
        :Type(aRght)
    {}
    
    TLocking(Type&& aRght)
        :Type(std::move(aRght))
    {}
};
//--------------------------------------------------------------

template<>
class TLocking <void>
{
public:
    virtual ~TLocking() = default;

    virtual void lock()
    {
        m_Locker.lock();
    }
    virtual void unlock()
    {
        m_Locker.unlock();
    }

private:
    std::mutex m_Locker;
};

}
}
