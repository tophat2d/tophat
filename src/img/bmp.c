#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "bmp.h"

int peekn(FILE *file, unsigned int n) {
	int tr = 0;

	for (unsigned int i=0; i < n; i++) {
		tr += (fgetc(file) << (8 * i));
	}

	return tr;
}

void parsebmp(char path[], img_t *img) {
	int cb;
	long int c;

	printf("%d\n", path[0]);

	FILE *f = fopen(&path[0], "r");
	if (f == NULL) {
		printf("could not open file\n");
		return;
	}

	while ((cb = fgetc(f)) != EOF) {
		c = ftell(f);

		printf("current byte: %d\n", c);

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
			printf("found sizeimage byte\n");
			img->sizeimage = cb + peekn(f, 3);
			break;
		default:

			if (img->sizeimage < 1) {
				break;
			}

			printf("starting img parsing\n");			

			img->content = malloc(img->sizeimage * sizeof(uint32_t));

			img->content[0] = cb;
	
			printf("%d\n", img->sizeimage);

			for (int i=1; i < img->sizeimage && (cb = fgetc(f)) != EOF; i++) {
				img->content[i] = (uint32_t)cb;
			}
			return;
			break;
		}
	}
}
