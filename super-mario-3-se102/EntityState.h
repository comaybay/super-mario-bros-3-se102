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
	void SetState(Handler handler);
	Handler GetState();
	void Update(float delta);

private:
	Handler stateHandler;
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
inline void EntityState<ENTITY>::SetState(Handler handler) {
	stateHandler = handler;
}

template<class ENTITY>
inline typename EntityState<ENTITY>::Handler EntityState<ENTITY>::GetState() {
	return stateHandler;
}

template<class ENTITY>
inline void EntityState<ENTITY>::Update(float delta) {
	(handlerThis->*stateHandler)(delta);
}
