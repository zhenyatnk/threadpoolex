#pragma once

#include <threadpoolex/core/IHandler.hpp>
#include <threadpoolex/core/export.hpp>

#include <list>
#include <memory>
#include <string>

namespace threadpoolex {
namespace core {

class IHandlerTimer
{
public:
    using Ptr = std::shared_ptr<IHandlerTimer>;

public:
    virtual ~IHandlerTimer() = default;

    virtual void OnCheck() = 0;
    virtual void OnClose() = 0;
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

    virtual void AddHandler(IHandlerTimer::Ptr aHandler) = 0;
    virtual void RemoveHandler(IHandlerTimer::Ptr aHandler) = 0;
};

class CObservableTimer
    :public virtual IObservableTimer, 
     public INotifierTimerClose,
     public INotifierTimerCheck
{
public:
    using Ptr = std::shared_ptr<CObservableTimer>;

public:
    virtual void AddHandler(IHandlerTimer::Ptr aHandler) override
    {
        m_ListHandler.push_back(aHandler);
    }

    virtual void RemoveHandler(IHandlerTimer::Ptr aHandler) override
    {
        m_ListHandler.remove(aHandler);
    }

    virtual void NotifyClose() override
    {
        for (auto lHandler : m_ListHandler)
            lHandler->OnClose();
    }
    
    virtual void NotifyCheck() override
    {
        for (auto lHandler : m_ListHandler)
            lHandler->OnCheck();
    }


private:
    std::list<IHandlerTimer::Ptr> m_ListHandler;
};

typedef CBaseObservable<IObservableTimer, CObservableTimer, IHandlerTimer> CBaseObservableTimer;

//------------------------------------------------------------------------------------------------------------------
class THREADPOOLEX_CORE_EXPORT EmptyHandlerTimer
    :public IHandlerTimer
{
public:
    virtual void OnCheck() override
    {}
    
    virtual void OnClose() override
    {}
};

}
}
//----------------------------------------------------------------------------------------------------------------------------------------
