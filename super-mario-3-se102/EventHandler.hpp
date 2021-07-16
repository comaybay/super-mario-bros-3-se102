#pragma once

template<class ...ARGS>
struct EventHandlerHasher {
	std::size_t operator()(const EventHandler<ARGS...>& eventHandler) const;
};

template<class ...ARGS>
inline std::size_t EventHandlerHasher<ARGS...>::operator()(const EventHandler<ARGS...>& eventHandler) const {
	return eventHandler.GetId();
}

template<class ...ARGS>
inline intptr_t EventHandler<ARGS...>::GetId() const {
	return id;
}
template<class ...ARGS>
inline void EventHandler<ARGS...>::Handle(ARGS... args) const {
	handler(args...);
}

template<class ...ARGS>
inline EventHandler<ARGS...>::EventHandler() {}

template<class ...ARGS>
inline EventHandler<ARGS...>::EventHandler(intptr_t id, std::function<void(ARGS...)> handler)
	: handler(handler), id(id)
{}

template<class ...ARGS>
inline bool EventHandler<ARGS...>::operator==(const EventHandler<ARGS...>& other) const {
	return id == other.id;
}

