#pragma once

#include <stdint.h>
#include <string>


static uint64_t constexpr mix(char m, uint64_t s) {
  return ((s<<7) + ~(s>>3)) + ~m;
 }
 
static uint64_t constexpr hash(const char * m) {
  return (*m) ? mix(*m,hash(m+1)) : 0;
 }
 
static uint64_t hash(std::string str) {
	return hash(const_cast<char*>(str.c_str()));
} 