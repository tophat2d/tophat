// vim: noet
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
