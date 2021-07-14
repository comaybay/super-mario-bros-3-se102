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

template <class DERIVED>
using LPMovement = Movement<DERIVED>*;

template <class DERIVED>
inline Movement<DERIVED>::Movement(LPDERIVED derivedThis, StateHandler initialState) :
	derivedThis(derivedThis),
	initialState(initialState),
	state(initialState),
	finished(false)
{}


template <class DERIVED>
inline void Movement<DERIVED>::Update(float delta)
{
	if (!finished)
		(derivedThis->*state)(delta);
}

template <class DERIVED>
inline bool Movement<DERIVED>::Finished() {
	return finished;
}

template <class DERIVED>
inline void Movement<DERIVED>::Reset()
{
	state = initialState;
	finished = false;
}

template <class DERIVED>
inline void Movement<DERIVED>::SwitchState(StateHandler state) {
	this->state = state;
}

template <class DERIVED>
inline void Movement<DERIVED>::SetMovementFinished() {
	finished = true;
}
