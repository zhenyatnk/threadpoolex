#pragma once

#include <threadpoolex/core/IObserver.hpp>
#include <threadpoolex/core/export.hpp>

#include <list>
#include <memory>
#include <string>

namespace threadpoolex {
namespace core {

class THREADPOOLEX_CORE_EXPORT IObserverTask
    :public IObserverError
{
public:
    using Ptr = std::shared_ptr<IObserverTask>;

public:
    virtual ~IObserverTask() = default;

    virtual void HandleStart() = 0;
    virtual void HandleComplete() = 0;
};

class THREADPOOLEX_CORE_EXPORT INotifierTaskStart
{
public:
    using Ptr = std::shared_ptr<INotifierTaskStart>;

public:
    virtual ~INotifierTaskStart() = default;

    virtual void NotifyStart() = 0;
};

class THREADPOOLEX_CORE_EXPORT INotifierTaskComplete
{
public:
    using Ptr = std::shared_ptr<INotifierTaskComplete>;

public:
    virtual ~INotifierTaskComplete() = default;

    virtual void NotifyComplete() = 0;
};

class THREADPOOLEX_CORE_EXPORT IObservableTask
{
public:
    virtual ~IObservableTask() = default;

    virtual void AddObserver(IObserverTask::Ptr aHandler) = 0;
    virtual void RemoveObserver(IObserverTask::Ptr aHandler) = 0;
};

class THREADPOOLEX_CORE_EXPORT CObservableTask
    :public virtual IObservableTask,
     public INotifierError,
     public INotifierTaskStart,
     public INotifierTaskComplete
{
public:
    using Ptr = std::shared_ptr<CObservableTask>;

public:
    virtual void AddObserver(IObserverTask::Ptr aHandler) override
    {
        m_ListObservers.push_back(aHandler);
    }

    virtual void RemoveObserver(IObserverTask::Ptr aHandler) override
    {
        m_ListObservers.remove(aHandler);
    }

    virtual void NotifyStart() override
    {
        for (auto lObserver : m_ListObservers)
            SAFE_CALL_BY_OBSERVER_LOG(lObserver->HandleStart(), this->NotifyError);
    }

    virtual void NotifyComplete() override
    {
        for (auto lObserver : m_ListObservers)
            SAFE_CALL_BY_OBSERVER_LOG(lObserver->HandleComplete(), this->NotifyError);
    }

    virtual void NotifyError(const std::string &aMessage, const int& aErrorCode) override
    {
        for (auto lObserver : m_ListObservers)
            lObserver->HandleError(aMessage, aErrorCode);
    }

private:
    std::list<IObserverTask::Ptr> m_ListObservers;
};

typedef CBaseObservable<IObservableTask, CObservableTask, IObserverTask> CBaseObservableTask;

//------------------------------------------------------------------------------------------------------------------
class THREADPOOLEX_CORE_EXPORT EmptyObserverTask
    :public IObserverTask
{
public:
    virtual void HandleStart() override
    {}

    virtual void HandleComplete() override
    {}

    virtual void HandleError(const std::string &aMessage, const int& aErrorCode) override
    {}
};

}
}
//----------------------------------------------------------------------------------------------------------------------------------------
