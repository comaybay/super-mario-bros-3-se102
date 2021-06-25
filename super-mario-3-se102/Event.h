#pragma once
#include <list>
#include <functional>
#include <unordered_map>
#include <string>
#include <type_traits>
#include "EventHandler.h"
#include "Entity.h"
#include "Utils.h"
#include "Contains.h"

template <class... Args>
class Event
{
public:
	~Event();
	template<class T>
	void Subscribe(T* handlerThis, void(T::* handler)(Args...));

	void Subscribe(void(*handler)(Args...));

	template<class T>
	void Unsubscribe(T* handlerThis, void(T::* handler)(Args...));

	void Unsubscribe(void(*handler)(Args...));

	void Notify(Args...);

private:
	template<typename R, typename T, typename U>
	std::function<R(Args...)> Attach(R(T::* f)(Args...), U p);

	template<class T>
	intptr_t GetAddressOf(void(T::* handler)(Args...));

	template<class T>
	intptr_t GetAddressOf(T* handlerThis);

	void OnEntityDestroy(LPEntity entity);
	std::unordered_map<intptr_t, std::list<LPEventHandler<Args...>>*> eventHandlersById;

	/// <summary>
	/// <para>To avoid iterator invalidation while notifying handlers, all operations regarding handler removal 
	/// will be beform after notifications is completed.</para>
	/// <para>When unsubscribe method is called, it will push necessary information for unsubscribing handler to a wait list,
	/// and after everything is safe (notifications is completed), will start performing handler unsubscribing (if there is any)
	/// by calling this method</para>
	/// </summary>
	void UnsubscribeFromEvent(intptr_t thisId, intptr_t handlerId);

	struct ToRemoveProps {
		intptr_t thisId;
		intptr_t handlerId;
		ToRemoveProps(intptr_t thisId, intptr_t handlerId) : thisId(thisId), handlerId(handlerId) {}
	};
	std::list<ToRemoveProps> unsubscribeWaitList;


	/// <summary>
	/// Since ordinary functions do not have "this", an id represent a null pointer will be used for all subscribed ordinary functions
	/// </summary>
	const intptr_t ORDINARY_FUNC_THIS_ID = 0;
};

template<class ...Args>
inline Event<Args...>::~Event()
{
	for (auto& pair : eventHandlersById)
		delete pair.second;
}

//taken from https://stackoverflow.com/questions/21192659/variadic-templates-and-stdbind
template<class ...Args>
template<typename R, typename T, typename U>
inline std::function<R(Args...)> Event<Args...>::Attach(R(T::* f)(Args...), U p)
{
	return [p, f](Args... args)->R { return (p->*f)(args...); };
};

template<class ...Args>
template<class T>
inline void Event<Args...>::Subscribe(T* handlerThis, void(T::* handler)(Args...))
{
	using namespace std::placeholders;
	std::function<void(Args...)> bindedHandler = Attach(handler, handlerThis);

	LPEventHandler<Args...> eventHandler = new EventHandler<Args...>(GetAddressOf(handler), bindedHandler);

	intptr_t tId = GetAddressOf(handlerThis);
	if (!Utils::Contains(tId, eventHandlersById))
		eventHandlersById[tId] = new std::list<LPEventHandler<Args...>>;

	eventHandlersById[tId]->push_back(eventHandler);

	//unregister when entity is destroy
	//if is other type, let user unsubscribe manually
	if (std::is_base_of<Entity, T>::value) {
		reinterpret_cast<LPEntity>(handlerThis)->GetDestroyEvent().Subscribe(this, &Event<Args...>::OnEntityDestroy);
	}
}

template<class ...Args>
inline void Event<Args...>::Subscribe(void(*handler)(Args...))
{
	using namespace std::placeholders;

	LPEventHandler<Args...> eventHandler = new EventHandler<Args...>(GetAddressOf(handler), handler);

	if (!Utils::Contains(ORDINARY_FUNC_THIS_ID, eventHandlersById))
		eventHandlersById[ORDINARY_FUNC_THIS_ID] = new std::list<LPEventHandler<Args...>>;

	eventHandlersById[ORDINARY_FUNC_THIS_ID]->push_back(eventHandler);
}

template<class ...Args>
template<class T>
inline void Event<Args...>::Unsubscribe(T* handlerThis, void(T::* handler)(Args...))
{
	intptr_t handlerId = GetAddressOf(handler);
	intptr_t thisId = GetAddressOf(handlerThis);
	unsubscribeWaitList.push_back(ToRemoveProps(thisId, handlerId));

	if (std::is_base_of<Entity, T>::value)
		reinterpret_cast<LPEntity>(handlerThis)->GetDestroyEvent().Unsubscribe(this, &Event<Args...>::OnEntityDestroy);
}

template<class ...Args>
inline void Event<Args...>::Unsubscribe(void(*handler)(Args...))
{
	intptr_t handlerId = GetAddressOf(handler);
	unsubscribeWaitList.push_back(ToRemoveProps(ORDINARY_FUNC_THIS_ID, handlerId));
}

template<class ...Args>
inline void Event<Args...>::Notify(Args... other)
{
	for (ToRemoveProps& props : unsubscribeWaitList)
		UnsubscribeFromEvent(props.thisId, props.handlerId);

	unsubscribeWaitList.clear();

	for (auto& pair : eventHandlersById)
		for (LPEventHandler<Args...>& eHandler : *pair.second)
			eHandler->Handle(other...);
}

template<class ...Args>
inline void Event<Args...>::OnEntityDestroy(LPEntity entity)
{
	//remove all entity handlers container from map (unsubscribe all entity handlers)
	auto it = eventHandlersById.find(reinterpret_cast<intptr_t>(&(*entity)));
	eventHandlersById.erase(it);
}

template<class ...Args>
inline void Event<Args...>::UnsubscribeFromEvent(intptr_t thisId, intptr_t handlerId)
{
	for (auto it = eventHandlersById[thisId]->begin(); it != eventHandlersById[thisId]->end(); it++)
		if ((*it)->GetId() == handlerId) {
			eventHandlersById[thisId]->erase(it);

			//completely remove container if container contains no handlers
			if (eventHandlersById[thisId]->size() == 0) {
				auto handlersIt = eventHandlersById.find(thisId);
				eventHandlersById.erase(handlersIt);
			}
			return;
		}

	throw std::exception("Unsubscribe failed: handler not found in Event");
}

template<class ...Args>
template<class T>
inline intptr_t Event<Args...>::GetAddressOf(void(T::* handler)(Args...))
{
	//explaination for the weird void*& cast: https://stackoverflow.com/questions/8121320/get-memory-address-of-member-function/8122891
	return reinterpret_cast<intptr_t>((void*&)handler);
}

template<class ...Args>
template<class T>
inline intptr_t Event<Args...>::GetAddressOf(T* handlerThis) {
	return reinterpret_cast<intptr_t>(handlerThis);
}

template<class ...Args>
using LPEvent = Event<Args...>*;

