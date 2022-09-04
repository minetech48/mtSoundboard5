#pragma once

#include <unordered_map>

template <typename K, typename V>
struct bimap {
	std::unordered_map<K, V*> valuesMap;
	std::unordered_map<V, K*> keysMap;
	
	inline void insert(K key, V value) {
		valuesMap.insert(key, &value);
		keysMap.insert(value, &key);
	}
	
	inline void removeValue(K key) {
		keysMap.erase(*valuesMap[key]);
		valuesMap.erase(key);
	}
	inline void removeKey(V value) {
		valuesMap.erase(*keysMap[value]);
		keysMap.erase(value);
	}
};