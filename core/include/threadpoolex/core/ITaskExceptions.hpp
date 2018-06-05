#pragma once
#include <threadpoolex/core/export.hpp>
#include <threadpoolex/core/BaseExceptions.hpp>

#include <string>

namespace threadpoolex {
namespace core {
namespace exceptions {

class THREADPOOLEX_CORE_EXPORT task_error
    :public exceptions::threadpoolex_error_base
{
public:
    task_error(const std::string &aMessage, const int &aErrorCode)
    :threadpoolex_error_base("Task error: " + aMessage, aErrorCode)
    {}
};

}
}
}
