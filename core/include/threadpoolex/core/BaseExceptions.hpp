#pragma once

#include <threadpoolex/core/export.hpp>
#include <baseex/core/BaseExceptions.hpp>

namespace threadpoolex {
namespace core {
namespace exceptions {

class THREADPOOLEX_CORE_EXPORT threadpoolex_error_base
    :public baseex::core::exceptions_base::error_base
{
public:
    threadpoolex_error_base(const std::string &aMessage, const int &aErrorCode)
        :baseex::core::exceptions_base::error_base(aMessage, aErrorCode)
    {}
};

}
}
}
