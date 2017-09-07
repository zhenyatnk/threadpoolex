#include <threadpoolex/core/ITimer.hpp>

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

class CTimer
    :public ITimer, public CBaseObservableTimer
{
public:
    explicit CTimer(unsigned int aIntervalMs);
    CTimer(unsigned int aIntervalMs, unsigned int aCountRepeat);

    virtual ~CTimer();

protected:
    void Run();

private:
    std::thread m_Timer;
    std::timed_mutex m_Stop;
    unsigned int m_IntervalMs;
    unsigned int m_CountRepeat;
    bool m_InfinityRepeat;
};

CTimer::CTimer(unsigned int aIntervalMs)
    :m_IntervalMs(aIntervalMs), m_InfinityRepeat(true), m_CountRepeat(0)
{
    Run();
}

CTimer::CTimer(unsigned int aIntervalMs, unsigned int aCountRepeat)
    :m_IntervalMs(aIntervalMs), m_InfinityRepeat(false), m_CountRepeat(aCountRepeat)
{
    Run();
}

CTimer::~CTimer()
{
    m_Stop.unlock();
    m_Timer.join();
    this->GetObserver()->NotifyClose();
}

void CTimer::Run()
{
    m_Stop.lock();
    m_Timer = std::thread([this](CObservableTimer::Ptr aObserver)
    {
        unsigned int lRepeated = 0;
        while ((m_InfinityRepeat || lRepeated < m_CountRepeat) &&
                !try_lock_for_ex(m_Stop, m_IntervalMs))
        {
            try
            {
                aObserver->NotifyCheck();
            }
            catch(...)
            {}

            lRepeated++;
        }

    }, this->GetObserver());
}

//---------------------------------------------------------------------------
ITimer::Ptr CreateTimer(unsigned int aIntervalMs)
{
    return std::make_shared<CTimer>(aIntervalMs);
}

ITimer::Ptr CreateTimer(unsigned int aIntervalMs, unsigned int aCountRepeat)
{
    return std::make_shared<CTimer>(aIntervalMs, aCountRepeat);
}

}
}