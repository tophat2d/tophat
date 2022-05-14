
#include <stdint.h>

uint32_t th_utf8_decode(uint32_t *out, char *c) {
	if (!(*c & 0x80)) {
		*out = *c;
		return 1;
	}

	int size = 1;
	for (; c[size] && (c[size] & 0xc0) == 0x80; size++)
		;

	uint32_t mask = 0;
	for (int i=0; i < 8 - size - 1; i++)
		mask |= 1 << i;

	uint32_t sum = 0;
	uint32_t shift = (size - 1) * 6;
	for (int i=0; i < size && c[i]; i++) {
		sum |= (c[i] & mask) << shift;
		if (i == 0) mask = 0x3f; // following bytes have 0011 1111 mask
		shift -= 6;
	}

	*out = sum;
	return size;
}
