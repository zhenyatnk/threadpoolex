#pragma once

#include <threadpoolex/core/ITimerActiveObserver.hpp>
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
THREADPOOLEX_CORE_EXPORT ITimerActive::Ptr CreateTimerActive(unsigned int aInterval);
THREADPOOLEX_CORE_EXPORT ITimerActive::Ptr CreateTimerActive(unsigned int aInterval, unsigned int aCountRepeat);

}
}