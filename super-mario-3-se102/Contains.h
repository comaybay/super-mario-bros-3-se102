#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Utils {
	template<class K, class V>
	bool Contains(const K& key, const std::unordered_map<K, V>& map);

	template<class K>
	bool Contains(const K& key, const std::unordered_set<K>& set);

	template<class T>
	bool Contains(const T& key, const std::vector<T>& vector);
	bool Contains(const char* key, const std::vector<std::string>& vector);

	template<class T>
	bool ContainsAnyOf(const std::vector<T>& elems, const std::vector<T>& vector);
}

template<class K, class V>
inline bool Utils::Contains(const K& key, const std::unordered_map<K, V>& map) {
	return (map.find(key) != map.end());
}

template<class T>
inline bool Utils::Contains(const T& key, const std::unordered_set<T>& set) {
	return (set.find(key) != set.end());
}

template<class T>
inline bool Utils::Contains(const T& elem, const std::vector<T>& vector) {
	return (std::find(vector.begin(), vector.end(), elem) != vector.end());
}

template<class T>
inline bool Utils::ContainsAnyOf(const std::vector<T>& elems, const std::vector<T>& vector) {
	for (auto& elem : elems)
		if (Utils::Contains(elem, vector))
			return true;

	return false;
}