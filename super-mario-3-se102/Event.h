#pragma once
#include <list>
#include <functional>
#include <map>
#include <string>
#include "Utils.h"
#include "EventHandler.h"
#include "Entity.h"
#include <type_traits>

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
	void OnEntityDestroy(LPEntity entity);
	std::map<intptr_t, std::list<LPEventHandler<Args...>>*> eventHandlersById;
};

template<class ...Args>
template<class T>
inline void Event<Args...>::Subscribe(T* handlerThis, void(T::* handler)(Args...))
{
	using namespace std::placeholders;
	std::function<void(Args...)> bindedHandler = Utils::Attach(handler, handlerThis);

	LPEventHandler<Args...> eventHandler = new EventHandler<Args...>(reinterpret_cast<intptr_t>(&handler), bindedHandler);

	intptr_t tId = reinterpret_cast<intptr_t>(&(*handlerThis));
	if (!Utils::MapHas(tId, eventHandlersById))
		eventHandlersById[tId] = new std::list<LPEventHandler<Args...>>;

	eventHandlersById[tId]->push_back(eventHandler);

	//unregister when entity is destroy
	//if is other type, let user unsubscribe manually
	if (std::is_base_of<Entity, T>::value) {
		LPEntity entity = (LPEntity)handlerThis;
		entity->GetDestroyEvent()->Subscribe(this, &Event<Args...>::OnEntityDestroy);
	}
}

template<class ...Args>
template<class T>
inline void Event<Args...>::Unsubscribe(T* handlerThis, void(T::* handler)(Args...))
{
	intptr_t handlerId = reinterpret_cast<intptr_t>(&handler);
	intptr_t tId = reinterpret_cast<intptr_t>(&(*handlerThis));
	for (auto it = eventHandlersById[tId]->begin(); it < eventHandlersById[tId]->end(); it++)
		if ((*it)->GetId() == handlerId) {
			eventHandlersById[tId]->erase(it);
		}
	throw std::exception("Unsubscribe failed: handler not found in Event");
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
}

template<class ...Args>
inline void Event<Args...>::OnEntityDestroy(LPEntity entity)
{
	//remove all entity handlers from map (unsubscribe all entity handlers)
	auto it = eventHandlersById.find(reinterpret_cast<intptr_t>(&(*entity)));
	eventHandlersById.erase(it);
}

template<class ...Args>
using LPEvent = Event<Args...>*;

