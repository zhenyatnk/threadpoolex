#pragma once

#include <memory>
#include <string>

namespace threadpoolex {
namespace core {

class IHandlerError
{
public:
    using Ptr = std::shared_ptr<IHandlerError>;

public:
    virtual ~IHandlerError() = default;

    virtual void OnError(const std::string &aMessage, const int& aErrorCode) = 0;
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

    virtual void AddHandler(typename HandlerIntf::Ptr aHandler) override
    {
        m_Observer->AddHandler(aHandler);
    }

    virtual void RemoveHandler(typename HandlerIntf::Ptr aHandler) override
    {
        m_Observer->RemoveHandler(aHandler);
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