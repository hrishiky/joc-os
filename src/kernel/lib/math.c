#include "math.h"

long long pow(int base, int exponent) {
	long long result = 1;

	for (int i = 0; i < exponent; i++) {
		result *= base;
	}

	return result;
}
