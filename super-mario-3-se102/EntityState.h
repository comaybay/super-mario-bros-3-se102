#pragma once

/// <summary>
/// type used for storing an entity's method pointer, use this implement basic finite state machine.
/// </summary>
template<class ENTITY>
class EntityState
{
public:
	typedef ENTITY* LPENTITY;
	typedef void(ENTITY::* StateHandler)(float);

	EntityState(LPENTITY entity);
	EntityState(LPENTITY entity, StateHandler intialHandler);
	void SetState(StateHandler handler);
	StateHandler GetState();
	void Update(float delta);

private:
	StateHandler stateHandler;
	LPENTITY handlerThis;
};


template<class ENTITY>
inline EntityState<ENTITY>::EntityState(LPENTITY entity, StateHandler intialHandler) :
	handlerThis(entity), stateHandler(intialHandler)
{
}

template<class ENTITY>
inline EntityState<ENTITY>::EntityState(LPENTITY entity) :
	handlerThis(entity), stateHandler(nullptr)
{
}

template<class ENTITY>
inline void EntityState<ENTITY>::SetState(StateHandler handler) {
	stateHandler = handler;
}

template<class ENTITY>
inline typename EntityState<ENTITY>::StateHandler EntityState<ENTITY>::GetState() {
	return stateHandler;
}

template<class ENTITY>
inline void EntityState<ENTITY>::Update(float delta) {
	(handlerThis->*stateHandler)(delta);
}
