#pragma once

#include <memory>
#include <functional>
#include <thread>
#include <atomic>

namespace threadpoolex {
namespace core {

//--------------------------------------------------------------
class IRAII
{
public:
    using Ptr = std::shared_ptr<IRAII>;

public:
    virtual ~IRAII() = default;
};

//--------------------------------------------------------------
template <typename Type>
class CRAII
    :public IRAII
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
    :public IRAII
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

template <>
class CRAII <void>
    :public IRAII
{
public:
    using TypeFunction = std::function<void()>;

public:
    CRAII(TypeFunction aCtor, TypeFunction aDtor)
        :m_Ctor(aCtor), m_Dtor(aDtor)
    {
        m_Ctor();
    }

    virtual ~CRAII()
    {
        m_Dtor();
    }

private:
    TypeFunction m_Ctor;
    TypeFunction m_Dtor;
};


//--------------------------------------------------------------
template <typename Type>
class CRAII <Type&&>
    :public IRAII
{
public:
    using TypeFunction = std::function<void(std::shared_ptr<Type>)>;

public:
    CRAII(Type&& aObj, TypeFunction aCtor, TypeFunction aDtor)
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
class force_lock_guard_ex
    :public CRAII <Type>
{
public:
    force_lock_guard_ex(Type& aObj)
        :CRAII(aObj, [](Type& aObj) {aObj.force_lock(); }, [](Type& aObj) {aObj.force_unlock(); })
    {}
};

template<typename Type>
class force_lock_guard_ex <std::shared_ptr<Type>>
    :public CRAII <std::shared_ptr<Type>>
{
public:
    force_lock_guard_ex(std::shared_ptr<Type> aObj)
        :CRAII(aObj, [](std::shared_ptr<Type> aObj) {aObj->force_lock(); }, [](std::shared_ptr<Type> aObj) {aObj->force_unlock(); })
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

//--------------------------------------------------------------
class thread_join_raii
    :public IRAII
{
private:
    class move_raii
        :public IRAII
    {
    public:
        explicit move_raii(std::thread&& aObj)
            :m_Obj(std::move(aObj))
        {}

        virtual ~move_raii()
        {
            if (m_Obj.joinable())
                m_Obj.join();
        }

    private:
        std::thread m_Obj;
    };

public:
    thread_join_raii()
        :m_RAII(nullptr)
    {}

    explicit thread_join_raii(std::shared_ptr<std::thread> aObj)
        :m_RAII(std::make_shared<CRAII<std::shared_ptr<std::thread>>>(aObj, [](std::shared_ptr<std::thread>& aObj) {}, [](std::shared_ptr<std::thread>& aObj) { if (aObj->joinable()) aObj->join(); }))
    {}

    explicit thread_join_raii(std::thread& aObj)
        :m_RAII(std::make_shared<CRAII<std::thread>>(aObj, [](std::thread& aObj) {}, [](std::thread& aObj) { if (aObj.joinable()) aObj.join(); }))
    {}

    explicit thread_join_raii(std::thread&& aObj)
        :m_RAII(std::make_shared<move_raii>(std::move(aObj)))
    {}

private:
    IRAII::Ptr m_RAII;
};



}
}