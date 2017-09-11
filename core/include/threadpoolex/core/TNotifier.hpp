#pragma once

#include <threadpoolex/core/RAII.hpp>

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
    TNotifier()
        :m_CountNotify(0), m_CountWait(0)
    {}

    virtual ~TNotifier() = default;

    virtual void notify_one()
    {
        std::unique_lock<std::mutex> lk(m_Mutex);
        m_CountNotify = 1;
        m_Condition.notify_one();
    }

    virtual void notify_all()
    {
        std::unique_lock<std::mutex> lk(m_Mutex);
        m_CountNotify = m_CountWait.load();
        m_Condition.notify_all();
    }

    virtual void wait()
    {
        wait([]() {});
    }

    template <typename Type>
    void wait(Type aFunction)
    {
        std::unique_lock<std::mutex> lk(m_Mutex);
        CRAII<std::atomic_int> lwait(m_CountWait, [](std::atomic_int& aValue) { ++aValue; }, [](std::atomic_int& aValue) { --aValue; });
        aFunction();
        m_Condition.wait(lk, [this]() {return m_CountNotify-- > 0; });
    }

private:
    std::condition_variable m_Condition;
    std::mutex m_Mutex;
    int m_CountNotify;
    std::atomic_int m_CountWait;
};

//--------------------------------------------------------------
}
}