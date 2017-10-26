#pragma once

#include <threadpoolex/core/export.hpp>

#include <memory>
#include <string>

namespace threadpoolex {
namespace core {

#define SAFE_CALL_BY_OBSERVER_LOG(func, catch_error) {try { func; } catch (std::exception &aError) { catch_error(aError.what(), 0);}}

class THREADPOOLEX_CORE_EXPORT IObserverError
{
public:
    using Ptr = std::shared_ptr<IObserverError>;

public:
    virtual ~IObserverError() = default;

    virtual void HandleError(const std::string &aMessage, const int& aErrorCode) = 0;
};

class THREADPOOLEX_CORE_EXPORT INotifierError
{
public:
    using Ptr = std::shared_ptr<INotifierError>;

public:
    virtual ~INotifierError() = default;

    virtual void NotifyError(const std::string &aMessage, const int& aErrorCode) = 0;
};

template <class ObservableIntf, class ObservableImpl, class HandlerIntf>
class CBaseObservable
    :public virtual ObservableIntf
{
public:
    CBaseObservable()
        :m_Observer(new ObservableImpl())
    {   }

    virtual void AddObserver(typename HandlerIntf::Ptr aHandler) override
    {
        m_Observer->AddObserver(aHandler);
    }

    virtual void RemoveObserver(typename HandlerIntf::Ptr aHandler) override
    {
        m_Observer->RemoveObserver(aHandler);
    }

protected:
    typename ObservableImpl::Ptr GetObserver() const 
    {
        return m_Observer;
    }

private:
    typename ObservableImpl::Ptr m_Observer;
};

}
}