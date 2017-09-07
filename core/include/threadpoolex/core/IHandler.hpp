#pragma once

#include <memory>
#include <string>

namespace threadpoolex {
namespace core {

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