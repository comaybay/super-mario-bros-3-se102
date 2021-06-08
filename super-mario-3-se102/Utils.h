#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <d3d9.h>
#include <dinput.h>
#include <functional>
#include <map>
#include <unordered_set>

namespace Utils
{
	void DebugOut(std::string text);
	void DebugOut(const char* text);
	struct Vector2
	{
		float x = 0;
		float y = 0;
		Vector2();
		Vector2(float x, float y);

		Vector2 operator+(const Vector2& other) const;
		Vector2 operator-(const Vector2& other) const;
		Vector2 operator-() const;
		Vector2& operator+=(const Vector2& other);
		Vector2& operator-=(const Vector2& other);
		Vector2 operator*(const Vector2& other) const;
		Vector2 operator*(int value) const;
		Vector2 operator*(float value) const;
		Vector2 Rounded();
		float DistanceTo(const Vector2& v);

	};

	struct Dimension {
		float width = 0;
		float height = 0;
		Dimension();
		Dimension(float width, float height);
		Dimension Rounded();
	};

	struct SpriteBox {
		RECT rect;
		Vector2 offset;
		SpriteBox(RECT rect, Vector2 offset) : rect(rect), offset(offset) {};
	};

	struct InvalidTokenSizeException : public std::exception {
		InvalidTokenSizeException(int numberOfArgsExpected);
		const char* what() const throw ();

	private:
		const char* message;
	};

	int Sign(int number);
	float Sign(float number);

	std::wstring StringToWideString(const std::string& s);

	std::string GetNextNonCommentLine(std::ifstream& file);

	std::vector<std::string> SplitByComma(const std::string& line);

	std::vector<std::string> SplitEvery(int splitLength, const std::string& line);

	std::vector<std::string> SplitEvery(int splitLength, const std::string& line);

	HWND CreateGameWindow(HINSTANCE hInstance, int nCmdShow, int ScreenWidth, int ScreenHeight);
	LPDIRECT3DDEVICE9 CreateDirect3DDevice(LPDIRECT3D9 d3d, HWND windowHandle);

	LPDIRECTINPUTDEVICE8 CreateDirectInputDevice(LPDIRECTINPUT8 di, HWND windowHandle, DWORD keyboardBufferSize);

	std::vector<SpriteBox> CreateSpriteBoxSequence(Vector2 startPosition, Dimension dimension, int space, int frameCount, Vector2 offset);

	std::string JoinPath(const std::string& path1, const std::string& path2);

	int HexCharToInt(const char& ch);

	template<class K, class V>
	bool MapHas(const K& key, const std::map<K, V>& map);

	template<class T>
	bool SetHas(const T& key, const std::unordered_set<T>& set);

	template<class T>
	bool VectorHas(const T& elem, const std::vector<T>& vector);

	template<class T>
	bool VectorHasAnyOf(const std::vector<T>& elems, const std::vector<T>& vector);

	template<typename R, typename T, typename U, typename... Args>
	std::function<R(Args...)> Attach(R(T::* f)(Args...), U p);

	template <typename T>
	inline T Clip(const T& n, const T& lower, const T& upper);

}

template<class K, class V>
inline bool Utils::MapHas(const K& key, const std::map<K, V>& map) {
	auto it = map.find(key);
	return (it != map.end());
}

template<class T>
inline bool Utils::SetHas(const T& key, const std::unordered_set<T>& set) {
	auto it = set.find(key);
	return (it != set.end());
}

//taken from https://stackoverflow.com/questions/21192659/variadic-templates-and-stdbind
template<typename R, typename T, typename U, typename... Args>
inline std::function<R(Args...)> Utils::Attach(R(T::* f)(Args...), U p)
{
	return [p, f](Args... args)->R { return (p->*f)(args...); };
};

template<class T>
inline bool Utils::VectorHas(const T& elem, const std::vector<T>& vector) {
	return (std::find(vector.begin(), vector.end(), elem) != vector.end());
}


template<class T>
inline bool Utils::VectorHasAnyOf(const std::vector<T>& elems, const std::vector<T>& vector) {
	for (auto& elem : elems)
		if (std::find(vector.begin(), vector.end(), elem) != vector.end())
			return true;

	return false;
}

template <typename T>
inline T Utils::Clip(const T& n, const T& lower, const T& upper) {
	return max(lower, min(n, upper));
}