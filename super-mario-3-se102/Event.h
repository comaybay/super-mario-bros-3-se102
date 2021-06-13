#pragma once
#include <list>
#include <functional>
#include <map>
#include <string>
#include <type_traits>
#include "Utils.h"
#include "EventHandler.h"
#include "Entity.h"

template <class... Args>
class Event
{
public:
	~Event();
	template<class T>
	void Subscribe(T* handlerThis, void(T::* handler)(Args...));

	template<class T>
	void Unsubscribe(T* handlerThis, void(T::* handler)(Args...));

	void Notify(Args...);

private:
	template<class T>
	intptr_t GetAddressOf(void(T::* handler)(Args...));

	template<class T>
	intptr_t GetAddressOf(T* handlerThis);

	void OnEntityDestroy(LPEntity entity);
	std::map<intptr_t, std::list<LPEventHandler<Args...>>*> eventHandlersById;


	struct UnsubscribeProps {
		intptr_t thisId;
		intptr_t handlerId;
		UnsubscribeProps(intptr_t thisId, intptr_t handlerId) : thisId(thisId), handlerId(handlerId) {}
	};
	std::list<UnsubscribeProps> removeWaitList;

	void RemoveFromEvent(intptr_t thisId, intptr_t handlerId);
};

template<class ...Args>
template<class T>
inline void Event<Args...>::Subscribe(T* handlerThis, void(T::* handler)(Args...))
{
	using namespace std::placeholders;
	std::function<void(Args...)> bindedHandler = Utils::Attach(handler, handlerThis);

	LPEventHandler<Args...> eventHandler = new EventHandler<Args...>(GetAddressOf(handler), bindedHandler);

	intptr_t tId = GetAddressOf(handlerThis);
	if (!Utils::MapHas(tId, eventHandlersById))
		eventHandlersById[tId] = new std::list<LPEventHandler<Args...>>;

	eventHandlersById[tId]->push_back(eventHandler);

	//unregister when entity is destroy
	//if is other type, let user unsubscribe manually
	if (std::is_base_of<Entity, T>::value) {
		reinterpret_cast<LPEntity>(handlerThis)->GetDestroyEvent()->Subscribe(this, &Event<Args...>::OnEntityDestroy);
	}
}

template<class ...Args>
template<class T>
inline void Event<Args...>::Unsubscribe(T* handlerThis, void(T::* handler)(Args...))
{
	intptr_t handlerId = GetAddressOf(handler);
	intptr_t thisId = GetAddressOf(handlerThis);

	if (std::is_base_of<Entity, T>::value)
		reinterpret_cast<LPEntity>(handlerThis)->GetDestroyEvent()->Unsubscribe(this, &Event<Args...>::OnEntityDestroy);

	removeWaitList.push_back(UnsubscribeProps(thisId, handlerId));
}

template<class ...Args>
inline Event<Args...>::~Event()
{
	for (auto& pair : eventHandlersById)
		delete pair.second;
}

template<class ...Args>
inline void Event<Args...>::Notify(Args... other)
{
	for (auto& pair : eventHandlersById)
		for (LPEventHandler<Args...>& eHandler : *pair.second)
			eHandler->Handle(other...);

	for (UnsubscribeProps& props : removeWaitList)
		RemoveFromEvent(props.thisId, props.handlerId);
}

template<class ...Args>
inline void Event<Args...>::OnEntityDestroy(LPEntity entity)
{
	//remove all entity handlers container from map (unsubscribe all entity handlers)
	auto it = eventHandlersById.find(reinterpret_cast<intptr_t>(&(*entity)));
	eventHandlersById.erase(it);
}

template<class ...Args>
inline void Event<Args...>::RemoveFromEvent(intptr_t thisId, intptr_t handlerId)
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

