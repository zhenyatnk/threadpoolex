#pragma once

#include <threadpoolex/core/export.hpp>
#include <threadpoolex/version/version.hpp>

#include <memory>

namespace threadpoolex {
namespace core {

//--------------------------------------------------------------
class ITimerPassive
{
public:
    using Ptr = std::shared_ptr<ITimerPassive>;

public:
    virtual ~ITimerPassive() = default;

    virtual bool Check() const = 0;
    virtual void Reset() = 0;
};
//--------------------------------------------------------------
THREADPOOLEX_CORE_EXPORT ITimerPassive::Ptr CreateTimerPassive(unsigned int aInterval);

}
}