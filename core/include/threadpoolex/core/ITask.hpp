#pragma once

#include <memory>

namespace threadpoolex {
namespace core {

class ITask
{
public:
    using Ptr = std::shared_ptr<ITask>;

public:
    virtual ~ITask() = default;

    virtual void Execite() = 0;
};

}
}