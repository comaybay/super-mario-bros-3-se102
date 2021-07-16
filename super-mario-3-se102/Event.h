#pragma once
#include <functional>
#include <unordered_map>
#include <string>
#include <type_traits>

#include "EventHandler.h"
#include "EventHandlerProps.h"
#include "Entity.h"
#include "Utils.h"
#include "Contains.h"

template <class... ARGS>
class Event
{
public:
	typedef void(*FuncHandler)(ARGS...);
	template <class T>
	using MethodHandler = void(T::*)(ARGS...);

	~Event();
	template<class T>
	void Subscribe(T* handlerThis, MethodHandler<T> handler);

	void Subscribe(FuncHandler handler);

	template<class T>
	void Unsubscribe(T* handlerThis, MethodHandler<T> handler);

	template<class T>
	void Unsubscribe(T* handlerThis);

	void Unsubscribe(FuncHandler handler);

	void Notify(ARGS...);

private:
	//taken from https://stackoverflow.com/questions/21192659/variadic-templates-and-stdbind
	template<typename R, typename T, typename U>
	std::function<R(ARGS...)> Attach(R(T::* f)(ARGS...), U p);

	template<class T>
	intptr_t GetAddressOf(MethodHandler<T>  handler);

	template<class T>
	intptr_t GetAddressOf(T* handlerThis);

	void OnEntityDestroy(LPEntity entity);

	typedef std::unordered_set<EventHandler<ARGS...>, EventHandlerHasher<ARGS...>> EventHandlerSet;
	std::unordered_map<intptr_t, EventHandlerSet*> eventHandlersById;

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

#include "Event.hpp"
