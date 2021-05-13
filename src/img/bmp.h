#ifndef BMP_H
#define BMP_H

typedef struct image {
	unsigned int size;
	unsigned int offset;
	unsigned int bsize;
	unsigned int w;
	unsigned int h;
	unsigned int bitcount;
	unsigned int sizeimage;
	uint32_t *content;

} img_t;

int peekn(FILE *file, unsigned int n);
void parsebmp(char *path, img_t *img);

#endif
