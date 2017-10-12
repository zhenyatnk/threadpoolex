#pragma once

#include <threadpoolex/core/IObserver.hpp>
#include <threadpoolex/core/export.hpp>

#include <list>
#include <memory>
#include <string>

namespace threadpoolex {
namespace core {

class IObserverTimer
    :public IObserverError
{
public:
    using Ptr = std::shared_ptr<IObserverTimer>;

public:
    virtual ~IObserverTimer() = default;

    virtual void HandleCheck() = 0;
    virtual void HandleClose() = 0;
};

class INotifierTimerClose
{
public:
    using Ptr = std::shared_ptr<INotifierTimerClose>;

public:
    virtual ~INotifierTimerClose() = default;

    virtual void NotifyClose() = 0;
};

class INotifierTimerCheck
{
public:
    using Ptr = std::shared_ptr<INotifierTimerCheck>;

public:
    virtual ~INotifierTimerCheck() = default;

    virtual void NotifyCheck() = 0;
};

class IObservableTimer
{
public:
    virtual ~IObservableTimer() = default;

    virtual void AddObserver(IObserverTimer::Ptr aHandler) = 0;
    virtual void RemoveObserver(IObserverTimer::Ptr aHandler) = 0;
};

class CObservableTimer
    :public virtual IObservableTimer,
     public INotifierError,
     public INotifierTimerClose,
     public INotifierTimerCheck
{
public:
    using Ptr = std::shared_ptr<CObservableTimer>;

public:
    virtual void AddObserver(IObserverTimer::Ptr aHandler) override
    {
        m_ListObservers.push_back(aHandler);
    }

    virtual void RemoveObserver(IObserverTimer::Ptr aHandler) override
    {
        m_ListObservers.remove(aHandler);
    }

    virtual void NotifyClose() override
    {
        for (auto lObserver : m_ListObservers)
            SAFE_CALL_BY_OBSERVER_LOG(lObserver->HandleClose(), this->NotifyError);
    }

    virtual void NotifyCheck() override
    {
        for (auto lObserver : m_ListObservers)
            SAFE_CALL_BY_OBSERVER_LOG(lObserver->HandleCheck(), this->NotifyError);
    }

    virtual void NotifyError(const std::string &aMessage, const int& aErrorCode) override
    {
        for (auto lObserver : m_ListObservers)
            lObserver->HandleError(aMessage, aErrorCode);
    }

private:
    std::list<IObserverTimer::Ptr> m_ListObservers;
};

typedef CBaseObservable<IObservableTimer, CObservableTimer, IObserverTimer> CBaseObservableTimer;

//------------------------------------------------------------------------------------------------------------------
class THREADPOOLEX_CORE_EXPORT EmptyObserverTimer
    :public IObserverTimer
{
public:
    virtual void HandleCheck() override
    {}

    virtual void HandleClose() override
    {}

    virtual void HandleError(const std::string &aMessage, const int& aErrorCode) override
    {}
};

}
}
//----------------------------------------------------------------------------------------------------------------------------------------
