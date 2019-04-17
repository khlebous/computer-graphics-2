public class EventSource
{
    public delegate void FooDelegate();
    public event FooDelegate FooEvent;

    private void RaiseFooEvent()
    {
        FooEvent();
    }
}

public class EventClient
{
    public void SubscribeAndUnsubscribe()
    {
        EventSource t = new EventSource();
        //subscribe via method name:
        t.FooEvent += HandleFooEvent;
        //unsubscribe via method name:
        t.FooEvent -= HandleFooEvent;

        //subscribe via lambda:
        t.FooEvent += () =>
        {
            HandleFooEvent();
        };
    }

    private void HandleFooEvent()
    {
    }
}
C++:

#include "tangible_event.h"

class EventSource
{
    using FooDelegate = std::function<void ()>;
public:
    TangibleEvent<FooDelegate> *FooEvent = new TangibleEvent<FooDelegate>();

    void RaiseFooEvent()
    {
        //invoke all listeners:
        for (auto listener : FooEvent->listeners())
        {
            listener();
        }
    }
};

class EventClient
{
public:
    void SubscribeAndUnsubscribe()
    {
        EventSource *t = new EventSource();
        //subscribe via method name:
        t->FooEvent->addListener(L"HandleFooEvent", [&] () {HandleFooEvent();});
        //unsubscribe via method name:
        t->FooEvent->removeListener(L"HandleFooEvent");

        //subscribe via lambda:
        t->FooEvent->addListener([&] ()
        {
            HandleFooEvent();
        });
    }

private:
    void HandleFooEvent()
    {
    }
};

//"tangible_event.h" follows:
//----------------------------------------------------------------------------------------
//	Copyright © 2007 - 2019 Tangible Software Solutions, Inc.
//	This class can be used by anyone provided that the copyright notice remains intact.
//
//	This class is used to convert C# events to C++.
//----------------------------------------------------------------------------------------
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

template<typename T>
class TangibleEvent final
{
private:
    std::unordered_map<std::wstring, T> namedListeners;
public:
    void addListener(const std::wstring &methodName, T namedEventHandlerMethod)
    {
        if (namedListeners.find(methodName) == namedListeners.end())
            namedListeners[methodName] = namedEventHandlerMethod;
    }
    void removeListener(const std::wstring &methodName)
    {
        if (namedListeners.find(methodName) != namedListeners.end())
            namedListeners.erase(methodName);
    }

private:
    std::vector<T> anonymousListeners;
public:
    void addListener(T unnamedEventHandlerMethod)
    {
        anonymousListeners.push_back(unnamedEventHandlerMethod);
    }

    std::vector<T> listeners()
    {
        std::vector<T> allListeners;
        for (auto listener : namedListeners)
        {
            allListeners.push_back(listener.second);
        }
        allListeners.insert(allListeners.end(), anonymousListeners.begin(), anonymousListeners.end());
        return allListeners;
    }
};
