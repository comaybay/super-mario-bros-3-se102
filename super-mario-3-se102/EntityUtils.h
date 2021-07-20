#pragma once

class Entity;
typedef Entity* LPEntity;

namespace EntityUtils
{
	/// <summary>
	/// Returns true if other entity is on left side of entity
	/// </summary>
	bool IsOnLeftSideOf(LPEntity entity, LPEntity other);

	/// <summary>
	/// Returns true if other entity is above entity
	/// </summary>
	bool IsAboveOf(LPEntity entity, LPEntity other);

	/// <summary>
	/// Taken from https://pastebin.com/zD1asN3v with modifications
	/// </summary>
	float Wave(float from, float to, float duration, float offset, float time);
};