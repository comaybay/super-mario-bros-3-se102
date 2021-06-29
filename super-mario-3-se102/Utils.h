#pragma once
#include <string>
#include <vector>

namespace Utils
{
	void DebugOut(std::string text);
	void DebugOut(const char* text);

	struct Dimension {
		float width = 0;
		float height = 0;
		Dimension();
		Dimension(float width, float height);
		Dimension operator/(float value) const;
		Dimension operator+(const Dimension& other) const;
		Dimension Rounded();
	};

	template <class T>
	struct Vector2
	{
		T x = 0;
		T y = 0;
		Vector2();
		Vector2(T x, T y);

		Vector2<T> operator+(const Vector2<T>& other) const;
		Vector2<T> operator+(const Dimension& other) const;
		Vector2<T> operator-(const Vector2<T>& other) const;
		Vector2<T> operator-(const Dimension& other) const;
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
		Vector2<int> Rounded() const;
		float DistanceTo(const Vector2<T>& v);
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
inline Utils::Vector2<int> Utils::Vector2<T>::Rounded() const {
	return Vector2<int>(round(x), round(y));
}

template <typename T>
inline float Utils::Vector2<T>::DistanceTo(const Utils::Vector2<T>& v)
{
	return sqrt(pow(x - v.x, 2) + pow(y - v.y, 2));
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator+(const Utils::Vector2<T>& other) const {
	return Vector2<T>(x + other.x, y + other.y);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator+(const Utils::Dimension& other) const {
	return Vector2<T>(x + other.width, y + other.height);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator-(const Utils::Vector2<T>& other) const {
	return Vector2<T>(x - other.x, y - other.y);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator-(const Utils::Dimension& other) const {
	return Vector2<T>(x - other.width, y - other.height);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator- () const {
	return  Vector2<T>(-x, -y);
}

template <typename T>
inline Utils::Vector2<T>& Utils::Vector2<T>::operator+=(const Utils::Vector2<T>& other) {
	x += other.x;
	y += other.y;
	return *this;
}

template <typename T>
inline Utils::Vector2<T>& Utils::Vector2<T>::operator-=(const Utils::Vector2<T>& other) {
	x -= other.x;
	y -= other.y;
	return *this;
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator*(const Utils::Vector2<T>& other) const {
	return Vector2<T>(x * other.x, y * other.y);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator*(int value) const {
	return Vector2<T>(x * value, y * value);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator*(float value) const {
	return Vector2<T>(x * value, y * value);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator/(const Utils::Vector2<T>& other) const {
	return Vector2<T>(x / other.x, y / other.y);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator/(int value) const {
	return Vector2<T>(x / value, y / value);
}

template <typename T>
inline Utils::Vector2<T> Utils::Vector2<T>::operator/(float value) const {
	return Vector2<T>(x / value, y / value);
}

template <typename T>
inline bool Utils::Vector2<T>::operator==(const Utils::Vector2<T>& other) const {
	return this->x == other.x && this->y == other.y;
}

template <typename T>
inline bool Utils::Vector2<T>::operator!=(const Utils::Vector2<T>& other) const {
	return !(*this == other);
}
