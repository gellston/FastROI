#pragma once


#include <stdexcept>

#ifndef FAST_MACRO
#define FAST_MACRO

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)
#pragma warning(disable : 4996)



#if FAST_API
#define FAST_API_EXPORT __declspec(dllexport)
#else
#define FAST_API_EXPORT __declspec(dllimport)
#endif


#define HVERROR(name, message) \
std::string name;name += "error:";name += __func__;name += ":";name += std::to_string(__LINE__);name += ":";name += message;throw std::runtime_error(name);\

#endif