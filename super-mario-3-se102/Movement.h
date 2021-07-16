#pragma once
template <class DERIVED>
class Movement {
public:
	typedef void(DERIVED::* StateHandler)(float);
	typedef DERIVED* LPDERIVED;

	virtual ~Movement() = default;
	Movement(LPDERIVED derivedThis, StateHandler initialState);

	void Update(float delta);
	bool Finished();
	void Reset();

protected:
	void SwitchState(StateHandler stateHandler);
	void SetMovementFinished();

private:
	StateHandler state;
	StateHandler initialState;
	LPDERIVED derivedThis;
	bool finished;
};

#include "Movement.hpp"