#include "util.h"

void iclamp(int* v, int mi, int mx) {
	if (*v < mi) *v = mi;
	else if (*v > mx) *v = mx;
}
