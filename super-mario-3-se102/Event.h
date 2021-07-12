#pragma once
#include <list>
#include <functional>
#include <unordered_map>
#include <string>
#include <type_traits>

#include "EventHandler.h"
#include "EventHandlerProps.h"
#include "Entity.h"
#include "Utils.h"
#include "Contains.h"

template <class... Args>
class Event
{
public:
	typedef void(*FuncHandler)(Args...);
	template <class T>
	using MethodHandler = void(T::*)(Args...);


	~Event();
	template<class T>
	void Subscribe(T* handlerThis, MethodHandler<T> handler);

	void Subscribe(FuncHandler handler);

	template<class T>
	void Unsubscribe(T* handlerThis, MethodHandler<T> handler);

	void Unsubscribe(FuncHandler handler);

	void Notify(Args...);

private:
	//taken from https://stackoverflow.com/questions/21192659/variadic-templates-and-stdbind
	template<typename R, typename T, typename U>
	std::function<R(Args...)> Attach(R(T::* f)(Args...), U p);

	template<class T>
	intptr_t GetAddressOf(MethodHandler<T>  handler);

	template<class T>
	intptr_t GetAddressOf(T* handlerThis);

	void OnEntityDestroy(LPEntity entity);
	std::unordered_map<intptr_t, std::unordered_set<EventHandler<Args...>, EventHandlerHasher<Args...>>*> eventHandlersById;

	/// <summary>
	/// <para>To avoid iterator invalidation while notifying handlers, all operations regarding handler removal 
	/// will be beform after notifications is completed.</para>
	/// <para>When unsubscribe method is called, it will push necessary information for unsubscribing handler to a wait list,
	/// and after everything is safe (notifications is completed), will start performing handler unsubscribing (if there is any)
	/// by calling this method</para>
	/// </summary>
	void UnsubscribeFromEvent(intptr_t thisId, intptr_t handlerId);
	std::unordered_set<EventHandlerProps, EventHandlerProps::Hasher> unsubscribeWaitList;

	/// <summary>
	/// When event is destroyed, This will be used to auto unsubscribe from entity's destroy events
	/// </summary>
	std::unordered_set<LPEntity> subscribedEntities;

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

	for (LPEntity entity : subscribedEntities)
		entity->GetDestroyEvent().Unsubscribe(this, &Event<Args...>::OnEntityDestroy);
}

template<class ...Args>
template<typename R, typename T, typename U>
inline std::function<R(Args...)> Event<Args...>::Attach(R(T::* f)(Args...), U p)
{
	return [p, f](Args... args)->R { return (p->*f)(args...); };
};

template<class ...Args>
template<class T>
inline void Event<Args...>::Subscribe(T* handlerThis, MethodHandler<T> handler)
{
	using namespace std::placeholders;
	std::function<void(Args...)> bindedHandler = Attach(handler, handlerThis);

	EventHandler<Args...> eventHandler = EventHandler<Args...>(GetAddressOf(handler), bindedHandler);

	intptr_t tId = GetAddressOf(handlerThis);
	if (!Utils::Contains(tId, eventHandlersById))
		eventHandlersById[tId] = new std::unordered_set<EventHandler<Args...>, EventHandlerHasher<Args...>>();

	eventHandlersById[tId]->insert(eventHandler);

	//unregister when entity is destroy
	//if is other type, let user unsubscribe manually
	if (std::is_base_of<Entity, T>::value) {
		LPEntity entity = reinterpret_cast<LPEntity>(handlerThis);
		entity->GetDestroyEvent().Subscribe(this, &Event<Args...>::OnEntityDestroy);
		subscribedEntities.insert(entity);
	}
}

template<class ...Args>
inline void Event<Args...>::Subscribe(FuncHandler handler)
{
	using namespace std::placeholders;

	EventHandler<Args...> eventHandler = EventHandler<Args...>(GetAddressOf(handler), handler);

	if (!Utils::Contains(ORDINARY_FUNC_THIS_ID, eventHandlersById))
		eventHandlersById[ORDINARY_FUNC_THIS_ID] = new std::unordered_set<EventHandler<Args...>, EventHandlerHasher<Args...>>();

	eventHandlersById[ORDINARY_FUNC_THIS_ID]->insert(eventHandler);
}

template<class ...Args>
template<class T>
inline void Event<Args...>::Unsubscribe(T* handlerThis, MethodHandler<T> handler)
{
	intptr_t handlerId = GetAddressOf(handler);
	intptr_t thisId = GetAddressOf(handlerThis);
	unsubscribeWaitList.insert(EventHandlerProps(thisId, handlerId));

	if (std::is_base_of<Entity, T>::value) {
		LPEntity entity = reinterpret_cast<LPEntity>(handlerThis);
		entity->GetDestroyEvent().Unsubscribe(this, &Event<Args...>::OnEntityDestroy);
		subscribedEntities.erase(entity);
	}
}

template<class ...Args>
inline void Event<Args...>::Unsubscribe(FuncHandler handler)
{
	intptr_t handlerId = GetAddressOf(handler);
	unsubscribeWaitList.insert(EventHandlerProps(ORDINARY_FUNC_THIS_ID, handlerId));
}

template<class ...Args>
inline void Event<Args...>::Notify(Args... other)
{
	for (const EventHandlerProps& props : unsubscribeWaitList)
		UnsubscribeFromEvent(props.thisId, props.handlerId);

	unsubscribeWaitList.clear();

	for (auto& pair : eventHandlersById)
		for (const EventHandler<Args...>& eHandler : *pair.second)
			eHandler.Handle(other...);
}

template<class ...Args>
inline void Event<Args...>::OnEntityDestroy(LPEntity entity)
{
	auto handlersIt = eventHandlersById.find(GetAddressOf(entity));
	eventHandlersById.erase(handlersIt);

	auto waitListIt = std::find_if(unsubscribeWaitList.begin(), unsubscribeWaitList.end(),
		[this, entity](const EventHandlerProps& props) -> bool { return props.thisId == GetAddressOf(entity); }
	);
	if (waitListIt != unsubscribeWaitList.end())
		unsubscribeWaitList.erase(waitListIt);
}

template<class ...Args>
inline void Event<Args...>::UnsubscribeFromEvent(intptr_t thisId, intptr_t handlerId)
{
	for (auto it = eventHandlersById[thisId]->begin(); it != eventHandlersById[thisId]->end(); it++)
		if ((*it).GetId() == handlerId) {
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
inline intptr_t Event<Args...>::GetAddressOf(MethodHandler<T>  handler)
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

