#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "bmp.h"

int peekn(FILE *file, unsigned int n) {
	int tr = 0;

	for (int i=0; i < n; i++) {
		tr += (fgetc(file) << (8 * i));
	}

	return tr;
}

void parsebmp(char *path, img_t *img) {
	int cb;
	long int c;

	printf("%d", *path);

	return;

	FILE *f = fopen(path, "r");

	while ((cb = fgetc(f)) != EOF) {
		c = ftell(f);

		switch (c) {
		case 3:
			img->size = cb + peekn(f, 3);
			break;
		case 11:
			img->offset = cb + peekn(f, 3);
			break;
		case 15:
			img->bsize = cb + peekn(f, 3);
			break;
		case 19:
			img->w = cb + peekn(f, 3);
			break;
		case 23:
			img->h = cb + peekn(f, 3);
			break;
		case 29:
			img->bitcount = cb + peekn(f, 1);
			break;
		case 35:
			img->sizeimage = cb + peekn(f, 3);
			break;
		default:
			img->content = malloc(img->sizeimage * sizeof(uint32_t));

			img->content[0] = cb;

			for (int i=0; i < img->sizeimage; i++) {
				img->content[i] = (uint32_t)fgetc(f);
			}
			return;
			break;
		}
	}
}
