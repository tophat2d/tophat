#ifndef CSV_H
#define CSV_H

typedef struct csv {
	char ***data;
	int rows;
	int *collumns;
} csv_t;

void getsize(int *w, int *h, char *inp);

char *gettocomma(int s, char *inp, int *le);

void csvparse(csv_t *res, char *inp);

char *tocsv(csv_t *inp);

#endif
