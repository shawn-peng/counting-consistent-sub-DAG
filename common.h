#include <stdio.h>

#define STD_ERROR_IN_FUNC \
	perror(__FUNCTION__)

#define FOR_EACH_IN_CONTAINER(iter, container)\
	for (auto iter = (container).begin();\
			iter != (container).end(); iter++)

#define FOR_EACH_IN_CONTAINER_REVERSE(iter, container)\
	for (auto iter = (container).rbegin();\
			iter != (container).rend(); iter++)

//#define DECLARE_ITERATER(iter, container)\
//	decltype((container).begin()) iter

