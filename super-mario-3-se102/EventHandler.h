#pragma once
#include <functional>
#include <string>

//avoid circular dependencies
class Entity;
typedef Entity* LPEntity;

template<class ...ARGS>
class EventHandler {
public:
	EventHandler(intptr_t id, const std::function<void(ARGS...)>& handler);
	intptr_t GetId() const;
	void Handle(ARGS... args) const;
	bool operator==(const EventHandler<ARGS...>& other) const;

private:
	intptr_t id;
	std::function<void(ARGS...)> handler;
};

#include "EventHandler.hpp"