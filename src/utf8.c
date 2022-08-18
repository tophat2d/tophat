#include <stdint.h>
#include <stddef.h>

size_t th_utf8_decode(uint32_t *out, const char *s_) {
	const unsigned char *s = (const unsigned char*)s_;
	if ((*s & 0xC0) != 0xC0) {
		*out = *s;
		return *s > 0;
	}

	const static size_t clas[32] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5};
	size_t cl = clas[*s>>3];

	for (size_t i = 1; i < cl; ++i) {
		if ((s[i] & 0xC0) == 0xC0 || (s[i] & 0x80) == 0) {
			*out = s[0];
			return 1;
		}
	}

	switch (cl) {
		case 2: *out = ((s[0]&0x1f)<<6) | (s[1]&0x3f); break;
		case 3: *out = ((s[0]&0xf)<<12) | ((s[1]&0x3f)<<6) | (s[2]&0x3f); break;
		case 4: *out = ((s[0]&0x7)<<18) | ((s[1]&0x3f)<<12) | ((s[2]&0x3f)<<6) | (s[3]&0x3f); break;
		// NOTE(skejeton): 5 octet sequences are not a part of UTF-8 standard, I've included them regardless 
		case 5: *out = ((s[0]&0x2)<<24) | ((s[0]&0x3f)<<18) | ((s[1]&0x3f)<<12) | ((s[2]&0x3f)<<6) | (s[3]&0x3f); break; 
	}

	return cl;
}

size_t th_utf8_encode(char *out, uint32_t r) {
	size_t s = 0;
	if (r < 0x7f) s = 1;
	else if (r < 0x7ff) s = 2;
	else if (r < 0xffff) s = 3;
	else if (r < 0x10ffff) s = 4;

	if (s == 1) {
		*out = (char)r;
		return s;
	}

	// header is size amount of ones at the start
	int header = 0;
	for (int i=0; i < s; i++) {
		header |= 1 << (7 - i);
	}

	int shift = (s - 1) * 6;
	out[0] = ((r >> shift & 0xff) | header) & 0xff;

	for (int i = 1; i < s; i++) {
		shift -= 6;
		out[i] = (r >> shift & 0x3f) | 0x80;
	}

	return s;
}
