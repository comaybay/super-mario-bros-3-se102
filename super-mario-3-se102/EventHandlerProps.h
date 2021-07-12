#pragma once
#include <cstddef>

struct EventHandlerProps {
	intptr_t thisId;
	intptr_t handlerId;

	EventHandlerProps(intptr_t thisId, intptr_t handlerId);

	bool operator==(const EventHandlerProps& other) const;

	struct Hasher {
		std::size_t operator()(const EventHandlerProps& props) const;
	};
};