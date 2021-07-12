#include "EventHandlerProps.h"
#include <functional>

EventHandlerProps::EventHandlerProps(intptr_t thisId, intptr_t handlerId) : thisId(thisId), handlerId(handlerId) {}

bool EventHandlerProps::operator==(const EventHandlerProps& other) const {
	return thisId == other.thisId && handlerId == other.handlerId;
}

//taken from https://stackoverflow.com/questions/23859844/hash-function-for-a-vector-of-pairint-int
std::size_t EventHandlerProps::Hasher::operator()(const EventHandlerProps& props) const {
	std::size_t seed = 0;
	//boost hash_combine implementation
	seed ^= std::hash<intptr_t>{}(props.thisId) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<intptr_t>{}(props.handlerId) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	return seed;
}
