#pragma once

#include <threadpoolex/version/version.hpp>
#include <mutex>

namespace threadpoolex {
namespace core {
//--------------------------------------------------------------
template<class Type>
class TLocking
    :public Type
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
//--------------------------------------------------------------
}
}