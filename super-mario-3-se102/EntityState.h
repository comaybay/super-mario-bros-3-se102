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

#include "EntityState.hpp"