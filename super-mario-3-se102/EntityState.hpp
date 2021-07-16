#pragma once

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
