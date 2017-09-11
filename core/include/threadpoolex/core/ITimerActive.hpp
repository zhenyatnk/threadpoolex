#pragma once

#include <threadpoolex/core/ITimerActiveHandler.hpp>
#include <threadpoolex/version/version.hpp>

#include <memory>

namespace threadpoolex {
namespace core {

//--------------------------------------------------------------
class ITimerActive
   :public virtual IObservableTimer
{
public:
    using Ptr = std::shared_ptr<ITimerActive>;

public:
    virtual ~ITimerActive() = default;
};
//--------------------------------------------------------------
ITimerActive::Ptr CreateTimerActive(unsigned int aInterval);
ITimerActive::Ptr CreateTimerActive(unsigned int aInterval, unsigned int aCountRepeat);

}
}