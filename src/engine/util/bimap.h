#pragma once

#include <unordered_map>
#include <vector>

template <typename K, typename V>
struct bimap {
	std::unordered_map<K, V> valuesMap;//todo: change implementation to prevent double storing elements
	std::unordered_map<V, K> keysMap;
	
	inline void insert(K key, V value) {
		if (valuesMap.contains(key)) {
			removeValue(key);
			removeKey(value);
		}
		valuesMap.insert({key, value});
		keysMap.insert({value, key});
	}
	
	inline void removeValue(K key) {
		keysMap.erase(valuesMap[key]);
		valuesMap.erase(key);
	}
	inline void removeKey(V value) {
		valuesMap.erase(keysMap[value]);
		keysMap.erase(value);
	}
	
	inline V getValue(K key) {
		if (!valuesMap.contains(key)) return {};
		
		return valuesMap[key];
	}
	inline K getKey(V value) {
		if (!keysMap.contains(value)) return {};
		
		return keysMap[value];
	}
	
	inline void clear() {
		valuesMap.clear();
		keysMap.clear();
	}
};