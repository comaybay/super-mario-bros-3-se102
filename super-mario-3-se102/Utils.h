#pragma once
#include <string>
#include <vector>
#include "Constants.h"

namespace Utils
{
	void DebugOut(std::string text);
	void DebugOut(const char* text);
	float Deg2Rad(float deg);
	float Rad2Deg(float rad);
	bool AlmostEqual(float a, float b);

	/// <summary>
	/// <para> Returns the angle a vector makes to an origin's X axis </para>
	/// <para> range from -180 (non-inclusive) to 180 (inclusive)</para>
	/// </summary>
	template <class T>
	struct Vector2;
	float AngleToXAxis(const Utils::Vector2<float>& vector, const Utils::Vector2<float>& orgin);

	template <class T>
	struct Dimension {
		T width = 0;
		T height = 0;
		Dimension();
		Dimension(T width, T height);

		template <class OTHER>
		Dimension(const Dimension<OTHER>& other);

		Dimension<T> operator/(float value) const;
		Dimension<T> operator*(int value) const;
		Dimension<T> operator*(float value) const;
		Dimension<T> operator+(const Dimension<T>& other) const;
		Dimension<T> Rounded() const;
	};

	template <class T>
	struct Vector2
	{
		T x = 0;
		T y = 0;
		Vector2();
		Vector2(T x, T y);

		template <class OTHER>
		Vector2(const Vector2<OTHER>& other);

		Vector2<T> operator+(const Vector2<T>& other) const;
		Vector2<T> operator+(const Dimension<T>& other) const;
		Vector2<T> operator-(const Vector2<T>& other) const;
		Vector2<T> operator-(const Dimension<T>& other) const;
		Vector2<T> operator-() const;
		Vector2<T>& operator+=(const Vector2<T>& other);
		Vector2<T>& operator-=(const Vector2<T>& other);
		Vector2<T> operator*(const Vector2<T>& other) const;
		Vector2<T> operator*(int value) const;
		Vector2<T> operator*(float value) const;
		Vector2<T> operator/(const Vector2<T>& other) const;
		Vector2<T> operator/(int value) const;
		Vector2<T> operator/(float value) const;
		bool operator==(const Vector2<T>& other) const;
		bool operator!=(const Vector2<T>& other) const;
		Vector2<T> Rounded() const;
		float DistanceTo(const Vector2<T>& v) const;

		/// <summary>
		///	Rotates vector to the given angle (counterclockwise rotation, 0 deg = rotates to x axis)
		/// </summary>
		Vector2<T> RotateTo(float angle) const;
	};

	template <typename T>
	int Sign(const T& number);

	template <typename T>
	T Clip(const T& n, const T& lower, const T& upper);

	template <class T>
	const T& Max(const T& a, const T& b);

	template <class T>
	const T& Min(const T& a, const T& b);
}

template <class T>
inline const T& Utils::Max(const T& a, const T& b) {
	return (a < b) ? b : a;
}

template <class T>
inline const T& Utils::Min(const T& a, const T& b) {
	return (a < b) ? a : b;
}

template <typename T>
inline T Utils::Clip(const T& n, const T& lower, const T& upper) {
	return Utils::Max(lower, Utils::Min(n, upper));
}

template <typename T>
int Utils::Sign(const T& number)
{
	return (number > 0) - (number < 0);
}

template <typename T>
inline Utils::Vector2<T>::Vector2() {};

template <typename T>
inline Utils::Vector2<T>::Vector2(T x, T y) : x(x), y(y) {};

template <typename T>
template <class OTHER>
inline Utils::Vector2<T>::Vector2(const Utils::Vector2<OTHER>& other)
	: x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {};

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::Rounded() const
{
	return Vector2<T>(static_cast<T>(round(x)), static_cast<T>(round(y)));
}

template <typename T>
inline float Utils::Vector2<T>::DistanceTo(const Utils::Vector2<T>& v) const
{
	return static_cast<float>(sqrt(pow(x - v.x, 2) + pow(y - v.y, 2)));
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator+(const Utils::Vector2<T>& other) const
{
	return Vector2<T>(x + other.x, y + other.y);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator+(const Utils::Dimension<T>& other) const
{
	return Vector2<T>(x + other.width, y + other.height);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator-(const Utils::Vector2<T>& other) const
{
	return Vector2<T>(x - other.x, y - other.y);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator-(const Utils::Dimension<T>& other) const
{
	return Vector2<T>(x - other.width, y - other.height);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator- () const
{
	return  Vector2<T>(-x, -y);
}

template <typename T>
inline Utils::Vector2<T>& Utils::Vector2<T>::operator+=(const Utils::Vector2<T>& other)
{
	x += other.x;
	y += other.y;
	return *this;
}

template <typename T>
inline Utils::Vector2<T>& Utils::Vector2<T>::operator-=(const Utils::Vector2<T>& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator*(const Utils::Vector2<T>& other) const
{
	return Vector2<T>(x * other.x, y * other.y);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator*(int value) const
{
	return Vector2<T>(x * value, y * value);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator*(float value) const
{
	return Vector2<T>(x * value, y * value);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator/(const Utils::Vector2<T>& other) const
{
	return Vector2<T>(x / other.x, y / other.y);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator/(int value) const
{
	return Vector2<T>(x / value, y / value);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator/(float value) const
{
	return Vector2<T>(x / value, y / value);
}

template <typename T>
inline bool Utils::Vector2<T>::operator==(const Utils::Vector2<T>& other) const
{
	return this->x == other.x && this->y == other.y;
}

template <typename T>
inline bool Utils::Vector2<T>::operator!=(const Utils::Vector2<T>& other) const
{
	return !(*this == other);
}

template <typename T>
Utils::Vector2<T> Utils::Vector2<T>::RotateTo(float angle) const
{
	float cosTheta = cosf(Deg2Rad(angle));
	float sinTheta = sinf(Deg2Rad(angle));

	return {
		x * cosTheta - y * sinTheta,
		-(x * sinTheta + y * cosTheta)
	};
}


template <typename T>
inline Utils::Dimension<T>::Dimension() {};

template <typename T>
inline Utils::Dimension<T>::Dimension(T width, T height) : width(width), height(height) {}

template <typename T>
template <class OTHER>
inline Utils::Dimension<T>::Dimension(const Dimension<OTHER>& other)
	: width(static_cast<T>(other.width)), height(static_cast<T>(other.height)) {}

template <typename T>
inline Utils::Dimension<T> Utils::Dimension<T>::operator/(float value) const {
	return Dimension(width / value, height / value);
}

template <typename T>
inline Utils::Dimension<T> Utils::Dimension<T>::operator*(float value) const {
	return Dimension(width * value, height * value);
}

template <typename T>
inline Utils::Dimension<T> Utils::Dimension<T>::operator*(int value) const {
	return Dimension(width * value, height * value);
}

template <typename T>
inline Utils::Dimension<T> Utils::Dimension<T>::operator+(const Utils::Dimension<T>& other) const {
	return Dimension(width + other.width, height + other.height);
}

template <typename T>
inline Utils::Dimension<T> Utils::Dimension<T>::Rounded() const {
	return Dimension<T>(static_cast<T>(round(width)), static_cast<T>(round(height)));
}

