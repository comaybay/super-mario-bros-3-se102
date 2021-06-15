#pragma once

/// <summary>
/// type used for storing an entity's method pointer, use this implement basic finite state machine.
/// </summary>
template<class ENTITY>
class EntityState
{
public:
	typedef ENTITY* LPENTITY;
	using Handler = void (ENTITY::*)(float);

	EntityState(LPENTITY entity);
	EntityState(LPENTITY entity, Handler intialHandler);
	void SetHandler(Handler handler);
	void Handle(float delta);

private:
	EntityState<ENTITY>::Handler stateHandler;
	LPENTITY handlerThis;
};


template<class ENTITY>
inline EntityState<ENTITY>::EntityState(LPENTITY entity, Handler intialHandler) {
	handlerThis = entity;
	stateHandler = intialHandler;
}

template<class ENTITY>
inline EntityState<ENTITY>::EntityState(LPENTITY entity) {
	handlerThis = entity;
}

template<class ENTITY>
inline void EntityState<ENTITY>::SetHandler(Handler handler) {
	stateHandler = handler;
}

template<class ENTITY>
inline void EntityState<ENTITY>::Handle(float delta) {
	(handlerThis->*stateHandler)(delta);
}
