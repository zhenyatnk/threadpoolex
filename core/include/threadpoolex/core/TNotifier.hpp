#pragma once

#include <condition_variable>

namespace threadpoolex {
namespace core {

template<class Type>
class TNotifier
    :public Type
{
public:
    void notify_one()
    {
        std::unique_lock<std::mutex> lk(m_Mutex);
        m_Condition.notify_one();
    }
    void notify_all()
    {
        std::unique_lock<std::mutex> lk(m_Mutex);
        m_Condition.notify_all();
    }

    void wait()
    {
        std::unique_lock<std::mutex> lk(m_Mutex);
        m_Condition.wait(lk);
    }

private:
    std::condition_variable m_Condition;
    std::mutex m_Mutex;
};

}
}