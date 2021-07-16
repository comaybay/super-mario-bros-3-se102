#pragma once

template<class ...ARGS>
inline Event<ARGS...>::~Event()
{
	for (auto& pair : eventHandlersById)
		delete pair.second;

	for (LPEntity entity : subscribedEntities)
		entity->GetDestroyEvent().Unsubscribe(this, &Event<ARGS...>::OnEntityDestroy);
}

template<class ...ARGS>
template<typename R, typename T, typename U>
inline std::function<R(ARGS...)> Event<ARGS...>::Attach(R(T::* f)(ARGS...), U p)
{
	return [p, f](ARGS... args)->R { return (p->*f)(args...); };
};

template<class ...ARGS>
template<class T>
inline void Event<ARGS...>::Subscribe(T* handlerThis, MethodHandler<T> handler)
{
	std::function<void(ARGS...)> bindedHandler = Attach(handler, handlerThis);

	intptr_t handlerId = GetAddressOf(handler);
	intptr_t tId = GetAddressOf(handlerThis);
	if (!Utils::Contains(tId, eventHandlersById))
		eventHandlersById[tId] = new EventHandlerSet();

	EventHandler<ARGS...> eventHandler = EventHandler<ARGS...>(handlerId, bindedHandler);
	eventHandlersById[tId]->insert(eventHandler);

	EventHandlerProps props(tId, handlerId);
	if (unsubscribeWaitList.find(props) != unsubscribeWaitList.end())
		unsubscribeWaitList.erase(props);

	//unregister when entity is destroy
	//if is other type, let user unsubscribe manually
	if (std::is_base_of<Entity, T>::value) {
		LPEntity entity = reinterpret_cast<LPEntity>(handlerThis);
		entity->GetDestroyEvent().Subscribe(this, &Event<ARGS...>::OnEntityDestroy);
		subscribedEntities.insert(entity);
	}
}

template<class ...ARGS>
inline void Event<ARGS...>::Subscribe(FuncHandler handler)
{
	EventHandler<ARGS...> eventHandler = EventHandler<ARGS...>(GetAddressOf(handler), handler);

	if (!Utils::Contains(ORDINARY_FUNC_THIS_ID, eventHandlersById))
		eventHandlersById[ORDINARY_FUNC_THIS_ID] = new EventHandlerSet();

	eventHandlersById[ORDINARY_FUNC_THIS_ID]->insert(eventHandler);

	EventHandlerProps props(ORDINARY_FUNC_THIS_ID, GetAddressOf(handler));
	if (unsubscribeWaitList.find(props) != unsubscribeWaitList.end())
		unsubscribeWaitList.erase(props);

}

template<class ...ARGS>
template<class T>
inline void Event<ARGS...>::Unsubscribe(T* handlerThis, MethodHandler<T> handler)
{
	intptr_t handlerId = GetAddressOf(handler);
	intptr_t thisId = GetAddressOf(handlerThis);
	unsubscribeWaitList.insert(EventHandlerProps(thisId, handlerId));

	if (std::is_base_of<Entity, T>::value) {
		LPEntity entity = reinterpret_cast<LPEntity>(handlerThis);
		entity->GetDestroyEvent().Unsubscribe(this, &Event<ARGS...>::OnEntityDestroy);
		subscribedEntities.erase(entity);
	}
}

template<class ...ARGS>
template<class T>
inline void Event<ARGS...>::Unsubscribe(T* handlerThis)
{
	intptr_t thisId = GetAddressOf(handlerThis);
	for (const EventHandler<ARGS...>& eventHandler : *eventHandlersById[thisId])
		unsubscribeWaitList.insert(EventHandlerProps(thisId, eventHandler.GetId()));

	if (std::is_base_of<Entity, T>::value) {
		LPEntity entity = reinterpret_cast<LPEntity>(handlerThis);
		entity->GetDestroyEvent().Unsubscribe(this, &Event<ARGS...>::OnEntityDestroy);
		subscribedEntities.erase(entity);
	}
}

template<class ...ARGS>
inline void Event<ARGS...>::Unsubscribe(FuncHandler handler)
{
	unsubscribeWaitList.insert(EventHandlerProps(ORDINARY_FUNC_THIS_ID, GetAddressOf(handler)));
}

template<class ...ARGS>
inline void Event<ARGS...>::Notify(ARGS... other)
{
	for (const EventHandlerProps& props : unsubscribeWaitList)
		UnsubscribeFromEvent(props.thisId, props.handlerId);

	unsubscribeWaitList.clear();

	for (auto& pair : eventHandlersById)
		for (const EventHandler<ARGS...>& eHandler : *pair.second)
			eHandler.Handle(other...);
}

template<class ...ARGS>
inline void Event<ARGS...>::OnEntityDestroy(LPEntity entity)
{
	auto handlersIt = eventHandlersById.find(GetAddressOf(entity));
	eventHandlersById.erase(handlersIt);

	subscribedEntities.erase(entity);

	auto waitListIt = std::find_if(unsubscribeWaitList.begin(), unsubscribeWaitList.end(),
		[this, entity](const EventHandlerProps& props) -> bool { return props.thisId == GetAddressOf(entity); }
	);
	if (waitListIt != unsubscribeWaitList.end())
		unsubscribeWaitList.erase(waitListIt);
}

template<class ...ARGS>
inline void Event<ARGS...>::UnsubscribeFromEvent(intptr_t thisId, intptr_t handlerId)
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

template<class ...ARGS>
template<class T>
inline intptr_t Event<ARGS...>::GetAddressOf(MethodHandler<T>  handler)
{
	//explaination for the weird void*& cast: https://stackoverflow.com/questions/8121320/get-memory-address-of-member-function/8122891
	return reinterpret_cast<intptr_t>((void*&)handler);
}

template<class ...ARGS>
template<class T>
inline intptr_t Event<ARGS...>::GetAddressOf(T* handlerThis) {
	return reinterpret_cast<intptr_t>(handlerThis);
}

template<class ...ARGS>
using LPEvent = Event<ARGS...>*;

