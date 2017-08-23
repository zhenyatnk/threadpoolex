#pragma once

#include <mutex>

namespace threadpoolex {
namespace core {

template<class Type>
class TLocking
    :public Type
{
public:
    void lock()
    {
        m_Locker.lock();
    }
    void unlock()
    {
        m_Locker.unlock();
    }

private:
    std::mutex m_Locker;
};

}
}