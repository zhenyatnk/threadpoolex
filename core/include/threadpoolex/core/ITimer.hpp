#pragma once

#include <threadpoolex/core/ITimerHandler.hpp>
#include <threadpoolex/version/version.hpp>

#include <memory>

namespace threadpoolex {
namespace core {

//--------------------------------------------------------------
class ITimer
   :public virtual IObservableTimer
{
public:
    using Ptr = std::shared_ptr<ITimer>;

public:
    virtual ~ITimer() = default;
};
//--------------------------------------------------------------
ITimer::Ptr CreateTimer(unsigned int aInterval);
ITimer::Ptr CreateTimer(unsigned int aInterval, unsigned int aCountRepeat);

}
}