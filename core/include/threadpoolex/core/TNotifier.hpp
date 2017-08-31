#pragma once

#include <condition_variable>
#include <atomic>

namespace threadpoolex {
namespace core {
//--------------------------------------------------------------
template<class Type>
class TNotifier
    :public Type, public TNotifier<void>
{
};
//--------------------------------------------------------------
template<>
class TNotifier <void>
{
public:
    virtual ~TNotifier() = default;

    virtual void notify_one()
    {
        std::unique_lock<std::mutex> lk(m_Mutex);
        m_Really = true;
        m_Condition.notify_one();
    }

    virtual void wait()
    {
        std::unique_lock<std::mutex> lk(m_Mutex);
        m_Condition.wait(lk, [this]() {return m_Really.exchange(false); });
    }

private:
    std::condition_variable m_Condition;
    std::mutex m_Mutex;
    std::atomic_bool m_Really;
};
//--------------------------------------------------------------
}
}