#pragma once

#include <functional>

namespace threadpoolex {
namespace core {

#define SINGLETON_NAME(CLASS, CREATER, NAME_CLASS) \
class NAME_CLASS \
{\
public:\
    static NAME_CLASS& GetInstance()\
    {\
        static NAME_CLASS gSingleton;\
        return gSingleton;\
    }\
public:\
    NAME_CLASS(const NAME_CLASS&) = delete;\
    NAME_CLASS& operator = (const NAME_CLASS&) = delete;\
    CLASS operator()() const\
    {\
        return m_Object;\
    }\
    CLASS& operator()()\
    {\
        return m_Object;\
    }\
private:\
    NAME_CLASS()\
        :m_Object(CREATER)\
    {}\
private:\
    CLASS m_Object;\
};

#define SINGLETON(CLASS, CREATER) SINGLETON_NAME(CLASS, CREATER, Singleton_##CLASS)

}
}
