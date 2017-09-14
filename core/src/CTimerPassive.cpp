#include <threadpoolex/core/ITimerPassive.hpp>
#include <threadpoolex/core/TNotifier.hpp>

#include <iostream>

#include <future>

namespace threadpoolex {
namespace core {

namespace
{
bool try_lock_for_ex(std::timed_mutex& aMutex, unsigned int aDuration)
{
#ifdef __linux__
    unsigned int aPart = 100;
    while (aDuration > aPart)
    {
        if (aMutex.try_lock())
            return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(aPart));
        aDuration -= aPart;
    }
    if (!!aDuration)
    {
        if (aMutex.try_lock())
            return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(aDuration));
    }
    return aMutex.try_lock();
#else
    if (!!aDuration)
        return aMutex.try_lock_for(std::chrono::milliseconds(aDuration));
    else
        return aMutex.try_lock();
#endif
}

}

class CTimerPassive
    :public ITimerPassive
{
public:
    explicit CTimerPassive(unsigned int aIntervalMs);

    virtual ~CTimerPassive();

    virtual bool Check() const override;
    virtual void Reset() override;

protected:
    void Run();
    void Stop();

private:
    thread_join_raii m_Timer;
    std::timed_mutex m_Stop;
    std::atomic_bool m_Check;
    unsigned int m_IntervalMs;
};

CTimerPassive::CTimerPassive(unsigned int aIntervalMs)
    :m_IntervalMs(aIntervalMs), m_Check(false)
{
    Run();
}

CTimerPassive::~CTimerPassive()
{
    Stop();
}

bool CTimerPassive::Check() const
{
    return m_Check;
}

void CTimerPassive::Reset()
{
    m_Check = false;
    Stop();
    Run();
}

void CTimerPassive::Run()
{
    m_Stop.try_lock();
    m_Timer = thread_join_raii(std::thread([this]()
    {
        if (!try_lock_for_ex(m_Stop, m_IntervalMs))
            m_Check = true;
    }));
}

void CTimerPassive::Stop()
{
    m_Stop.unlock();
    m_Timer = thread_join_raii();
}

//---------------------------------------------------------------------------
ITimerPassive::Ptr CreateTimerPassive(unsigned int aIntervalMs)
{
    return std::make_shared<CTimerPassive>(aIntervalMs);
}

}
}