#pragma once
#include <functional>
#include <string>

//avoid circular dependencies
class Entity;
typedef Entity* LPEntity;

template<class ...Args>
class EventHandler {
public:
	EventHandler();
	EventHandler(intptr_t id, std::function<void(Args...)> handler);
	intptr_t GetId() const;
	void Handle(Args... args) const;
	bool operator==(const EventHandler<Args...>& other) const;

private:
	intptr_t id;
	std::function<void(Args...)> handler;
};

template<class ...Args>
struct EventHandlerHasher {
	std::size_t operator()(const EventHandler<Args...>& eventHandler) const;
};

template<class ...Args>
inline intptr_t EventHandler<Args...>::GetId() const {
	return id;
}
template<class ...Args>
inline void EventHandler<Args...>::Handle(Args... args) const {
	handler(args...);
}

template<class ...Args>
inline EventHandler<Args...>::EventHandler() {}

template<class ...Args>
inline EventHandler<Args...>::EventHandler(intptr_t id, std::function<void(Args...)> handler)
	: handler(handler), id(id)
{}

template<class ...Args>
inline bool EventHandler<Args...>::operator==(const EventHandler<Args...>& other) const {
	return id == other.id;
}

template<class ...Args>
inline std::size_t EventHandlerHasher<Args...>::operator()(const EventHandler<Args...>& eventHandler) const {
	return eventHandler.GetId();
}