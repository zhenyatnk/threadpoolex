#pragma once

#include <threadpoolex/core/BaseExceptions.hpp>

namespace threadpoolex {
namespace core {
namespace exceptions {

class task_error
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