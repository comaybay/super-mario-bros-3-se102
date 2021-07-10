#pragma once
#include <functional>
#include <string>

//avoid circular dependencies
class Entity;
typedef Entity* LPEntity;

template<class ...Args>
class EventHandler {
private:
	intptr_t id;
	std::function<void(Args...)> handler;
public:
	uint64_t GetId() {
		return id;
	}

	void Handle(Args... args) {
		handler(args...);
 	}

	EventHandler() {}
	EventHandler(intptr_t id, std::function<void(Args...)> handler)
		: handler(handler), id(id)
	{}
};

template<class ...Args>
using LPEventHandler = EventHandler<Args...>*;