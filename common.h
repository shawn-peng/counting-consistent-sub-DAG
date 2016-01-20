#include <stdio.h>

#define STD_ERROR_IN_FUNC \
	perror(__FUNCTION__)

#define FOR_EACH_IN_CONTAINER(iter, container)\
	for (typeof((container).begin()) iter = (container).begin();\
			iter != (container).end(); iter++)

#define FOR_EACH_IN_CONTAINER_REVERSE(iter, container)\
	for (typeof((container).rbegin()) iter = (container).rbegin();\
			iter != (container).rend(); iter++)

#define DECLARE_ITERATER(iter, container)\
	typeof((container).begin()) iter

