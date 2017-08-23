#pragma once

#include <memory>
#include <functional>
#include <atomic>

namespace threadpoolex {
namespace core {

//--------------------------------------------------------------
template <typename Type>
class CRAII
{
public:
    using TypeFunction = std::function<void(Type&)>;

public:
    CRAII(Type& aObj, TypeFunction aCtor, TypeFunction aDtor)
        :m_Obj(aObj), m_Ctor(aCtor), m_Dtor(aDtor)
    {
        m_Ctor(m_Obj);
    }

    virtual ~CRAII()
    {
        m_Dtor(m_Obj);
    }

private:
    Type& m_Obj;
    TypeFunction m_Ctor;
    TypeFunction m_Dtor;
};
//--------------------------------------------------------------
template <typename Type>
class CRAII <std::shared_ptr<Type>>
{
public:
    using TypeFunction = std::function<void(std::shared_ptr<Type>)>;

public:
    CRAII(std::shared_ptr<Type> aObj, TypeFunction aCtor, TypeFunction aDtor)
        :m_Obj(aObj), m_Ctor(aCtor), m_Dtor(aDtor)
    {
        m_Ctor(m_Obj);
    }

    virtual ~CRAII()
    {
        m_Dtor(m_Obj);
    }

private:
    std::shared_ptr<Type> m_Obj;
    TypeFunction m_Ctor;
    TypeFunction m_Dtor;
};

//--------------------------------------------------------------
template<typename Type>
class lock_guard_ex
    :public CRAII <Type>
{
public:
    lock_guard_ex(Type& aObj)
        :CRAII(aObj, [](Type& aObj) {aObj.lock(); }, [](Type& aObj) {aObj.unlock(); })
    {}
};

template<typename Type>
class lock_guard_ex <std::shared_ptr<Type>>
    :public CRAII <std::shared_ptr<Type>>
{
public:
    lock_guard_ex(std::shared_ptr<Type> aObj)
        :CRAII(aObj, [](std::shared_ptr<Type> aObj) {aObj->lock(); }, [](std::shared_ptr<Type> aObj) {aObj->unlock(); })
    {}
};
//--------------------------------------------------------------
template<typename Type>
class set_values_raii
    :public CRAII <Type>
{
public:
    set_values_raii(Type& aObj, Type aFirst, Type aSecond)
        :CRAII(aObj, [aFirst](Type& aObj) {aObj = aFirst; }, [aSecond](Type& aObj) {aObj = aSecond; })
    {}
};

template<typename Type>
class set_values_raii <std::atomic<Type>>
    :public CRAII <std::atomic<Type>>
{
public:
    set_values_raii(std::atomic<Type>& aObj, Type aFirst, Type aSecond)
        :CRAII(aObj, [aFirst](std::atomic<Type>& aObj) {aObj = aFirst; }, [aSecond](std::atomic<Type>& aObj) {aObj = aSecond; })
    {}
};

}
}