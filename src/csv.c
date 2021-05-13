#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "csv.h"

////////////////////////////////
// parser

void getsize(int *w, int *h, char *inp) {
	int len = strlen(inp);
	int r = 0;
	w[r] = 0;
	for (int i=0; i < len; i++) {
		if (inp[i] == '\n') {
			w[r]++;
			r++;
			w[r] = 0;
		}
		if (inp[i] == ',')
			w[r]++;
	}
	*h = r;
}

char *gettocomma(int s, char *inp, int *le) {
	int l;
	int len = strlen(inp);
	char *tr;
	for (l=0; l < len && inp[s+l] != ',' && inp[s+l] != '\n'; l++)
	tr = malloc(sizeof(char) * (l + 1));
	for (int i=0; i < l; i++)
		tr[i] = inp[i+s];
	*le += l - 1;
	return tr;
}

void csvparse(csv_t *res, char *inp) {
	int *w = malloc(sizeof(int) * 256);
	int h = 0;
	int len = strlen(inp);
	getsize(&w[0], &h, inp);
	char ***out = malloc(sizeof(char **) * h);	
	int r = 0;
	int c = 1;
	out[0] = malloc(sizeof(char *) * w[0]);
	out[0][0] = gettocomma(0, inp, &r);
	r = 0;
	for (int i=0; i < len; i++) {
		if (inp[i] == '\n') {
			r++;
			c = 1;
			out[r] = malloc(sizeof(char *) * w[r]);
			out[r][0] = gettocomma(i+1, inp, &i);
			i++;
		}
		if (inp[i] == ',') {
			out[r][c] = gettocomma(i+1, inp, &i);
			c++;
		}
	}
	res->collumns = w;
	res->rows = h;
	res->data = out;
}

////////////////////////////////
// encoding

char *tocsv(csv_t *inp) {
	int size = 0;
	for (int i=0; i < inp->rows; i++) {
		for (int j=0; j < inp->collumns[i]; j++) {
			size += strlen(inp->data[i][j]);
		}
		size += inp->collumns[i];
	}
	size += inp->rows + 1;
	char *out = malloc(sizeof(char) * size);
	for (int i=0; i < inp->rows; i++) {
		for (int j=0; j < inp->collumns[i]; j++) {
			strcat(out, inp->data[i][j]);
			if (j < inp->collumns[i]-1)
				strcat(out, ",");
		}
		strcat(out, "\n");
	}
	return out;
}
