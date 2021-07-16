#pragma once

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
