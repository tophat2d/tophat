#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

		printf("current byte: %ld\n", c);

		switch (c) {
		case 2:
			img->size = cb + peekn(f, 3);
			break;
		case 10:
			img->offset = cb + peekn(f, 3);
			break;
		case 14:
			img->bsize = cb + peekn(f, 3);
			break;
		case 18:
			img->w = cb + peekn(f, 3);
			break;
		case 22:
			img->h = cb + peekn(f, 3);
			break;
		case 28:
			img->bitcount = cb + peekn(f, 1);
			break;
		case 34:
			printf("found sizeimage byte\n");
			img->sizeimage = cb + peekn(f, 3);
			break;
		default:

			if (c != img->offset + 1) {
				break;
			}

			if (img->sizeimage < 1) {
				break;
			}

			printf("starting img parsing\n");

			img->content = malloc(img->sizeimage * sizeof(uint32_t));

			img->content[0] = cb;

			int counter = 0;
			uint32_t number;
			int pixelsum = 0;
			for (int i=1; i < img->sizeimage && (cb = fgetc(f)) != EOF; i++) {
				number *= 256;
				number += (uint32_t)cb;
				counter++;

				if (counter >= 3) {
					number *= 256;
					number += 255;
					printf("%d\n", number);
					img->content[i/3] = number;
					number = 0;
					counter = 0;

					pixelsum++;
				}


			}

			//printf("%d\n", pixelsum);

			return;
			break;
		}
	}
}
