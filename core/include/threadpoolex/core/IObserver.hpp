#pragma once

#include <memory>
#include <string>

namespace threadpoolex {
namespace core {

class IObserverError
{
public:
    using Ptr = std::shared_ptr<IObserverError>;

public:
    virtual ~IObserverError() = default;

    virtual void HandleError(const std::string &aMessage, const int& aErrorCode) = 0;
};

class INotifierError
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