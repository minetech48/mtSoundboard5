#ifndef VEC2_I_H
#define VEC2_I_H

#include <yaml-cpp/yaml.h>

struct vec2i {
	vec2i() {}
	vec2i(int x, int y) : x(x), y(y) {}
	
	int x = 0, y = 0;
	
	//operators
	vec2i operator+(vec2i &vec2) {
		return vec2i(
			this->x + vec2.x,
			this->y + vec2.x
		);
	}
	
	vec2i& operator+=(vec2i &vec2) {
		this->x+= vec2.x;
		this->y+= vec2.y;
		
		return *this;
	}
	
	vec2i operator-(vec2i &vec2) {
		return vec2i(
			this->x - vec2.x,
			this->y - vec2.x
		);
	}
	
	vec2i& operator-=(vec2i &vec2) {
		this->x-= vec2.x;
		this->y-= vec2.y;
		
		return *this;
	}
	
	
	
	vec2i operator*(vec2i &vec2) {
		return vec2i(
			this->x * vec2.x,
			this->y * vec2.x
		);
	}
	
	vec2i& operator*=(vec2i &vec2) {
		this->x*= vec2.x;
		this->y*= vec2.y;
		
		return *this;
	}
	vec2i& mul(int mul) {
		this->x*= mul;
		this->y*= mul;
		
		return *this;
	}
};

//YAML compat
namespace YAML {
	template<>
	struct convert<vec2i> {
	static Node encode(const vec2i& rhs) {
		Node node;
		node.push_back(rhs.x);
		node.push_back(rhs.y);
		return node;
	}

	static bool decode(const Node& node, vec2i& rhs) {
		if(!node.IsSequence() || node.size() != 2) {
		return false;
		}

		rhs.x = node[0].as<int>();
		rhs.y = node[1].as<int>();
		return true;
	}
};
}
#endif