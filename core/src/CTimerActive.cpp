#include <threadpoolex/core/ITimerActive.hpp>

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

class CTimerActive
    :public ITimerActive, public CBaseObservableTimer
{
public:
    explicit CTimerActive(unsigned int aIntervalMs);
    CTimerActive(unsigned int aIntervalMs, unsigned int aCountRepeat);

    virtual ~CTimerActive();

protected:
    void Run();

private:
    std::thread m_Timer;
    std::timed_mutex m_Stop;
    unsigned int m_IntervalMs;
    unsigned int m_CountRepeat;
    bool m_InfinityRepeat;
};

CTimerActive::CTimerActive(unsigned int aIntervalMs)
    :m_IntervalMs(aIntervalMs), m_InfinityRepeat(true), m_CountRepeat(0)
{
    Run();
}

CTimerActive::CTimerActive(unsigned int aIntervalMs, unsigned int aCountRepeat)
    :m_IntervalMs(aIntervalMs), m_InfinityRepeat(false), m_CountRepeat(aCountRepeat)
{
    Run();
}

CTimerActive::~CTimerActive()
{
    m_Stop.unlock();
    m_Timer.join();
    this->GetObserver()->NotifyClose();
}

void CTimerActive::Run()
{
    m_Stop.try_lock();
    m_Timer = std::thread([this](CObservableTimer::Ptr aObserver)
    {
        unsigned int lRepeated = 0;
        while ((m_InfinityRepeat || lRepeated < m_CountRepeat) &&
                !try_lock_for_ex(m_Stop, m_IntervalMs))
        {
            aObserver->NotifyCheck();
            ++lRepeated;
        }

    }, this->GetObserver());
}

//---------------------------------------------------------------------------
ITimerActive::Ptr CreateTimerActive(unsigned int aIntervalMs)
{
    return std::make_shared<CTimerActive>(aIntervalMs);
}

ITimerActive::Ptr CreateTimerActive(unsigned int aIntervalMs, unsigned int aCountRepeat)
{
    return std::make_shared<CTimerActive>(aIntervalMs, aCountRepeat);
}

}
}