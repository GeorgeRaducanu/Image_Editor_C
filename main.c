//Copyright Raducanu George-Cristian 321CA
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LHELP (1 << 14)
#define LBUFF (1 << 7)

typedef struct image {
	int n, m;
	int x1, y1, x2, y2;
	int exists_loaded, is_gray, type, max_val;
	// type 2, 3, 5, 6
	int **r;
	int **g;
	int **b;
} image;

void free_img(image *img)
{
	if (img->exists_loaded == 1) {
		if (img->is_gray == 0) {
			for (int i = 0; i < img->m; ++i) {
				free(img->r[i]);
				free(img->b[i]);
				free(img->g[i]);
			}
			free(img->r);
			free(img->b);
			free(img->g);
		} else {
			for (int i = 0; i < img->m; ++i)
				free(img->r[i]);
			free(img->r);
		}
	}
}

void load_function(char filename[], image *img)
{
	FILE *in;
	if (filename[strlen(filename) - 1] == '\n')
		strcpy(filename + strlen(filename) - 1, filename + strlen(filename));
	in = fopen(filename, "rt");
	if (!in) {
		printf("Failed to load %s\n", filename);
		free_img(img);
		img->exists_loaded = 0;
	} else {
		char mini_buff[LBUFF];
		fgets(mini_buff, LBUFF, in);
		int tip = (int)(mini_buff[1] - '0');
		img->type = tip;
		fgets(mini_buff, LBUFF, in);
		while (mini_buff[0] == '#')
			fgets(mini_buff, LBUFF, in);
		char *aux_buff = mini_buff;
		char *p = strtok(aux_buff, " ");
		int n = atoi(p);
		int N = n;
		p = strtok(NULL, " ");
		int m = atoi(p);
		int M = m;
		if (N == 0 || M == 0) {
			printf("Failed to load %s\n", filename);
			free_img(img);
		img->exists_loaded = 0;
		return;
		}
		fgets(mini_buff, LBUFF, in);
		while (mini_buff[0] == '#')
			fgets(mini_buff, LBUFF, in);
		char *tok = strtok(mini_buff, "\n");
		int size = atoi(tok);
		img->max_val = size;
		fpos_t position1;
		fgetpos(in, &position1);

		fgets(mini_buff, LBUFF, in);
		while (mini_buff[0] == '#') {
			fgetpos(in, &position1);
			fgets(mini_buff, LBUFF, in);
		}
		fclose(in);
		free_img(img);
		img->n = N;
		img->m = M;
		// now new sizes must be allocated
		if (img->type == 2 || img->type == 5) {
			img->r = malloc((img->m) * sizeof(int *));
			for (int i = 0; i < img->m; ++i)
				img->r[i] = calloc((img->n), sizeof(int));

			img->is_gray = 1;
		} else {
			img->g = malloc((img->m) * sizeof(int *));
			for (int i = 0; i < img->m; ++i)
				img->g[i] = calloc((img->n), sizeof(int));

			img->r = malloc((img->m) * sizeof(int *));
			for (int i = 0; i < img->m; ++i)
				img->r[i] = calloc((img->n), sizeof(int));

			img->b = malloc((img->m) * sizeof(int *));
			for (int i = 0; i < img->m; ++i)
				img->b[i] = calloc((img->n), sizeof(int));

			img->is_gray = 0;
		}
		// 2, 3 ascii
		// 5, 6 binary
		// in position1 there is now the current cursor
		if (img->type == 2 || img->type == 3) {
			// ascii;
			int help;
			in = fopen(filename, "rt");
			fsetpos(in, &position1);
			if (img->type == 2) {
				// gray
				for (int i = 0; i < img->m; ++i) {
					for (int j = 0; j < img->n; ++j) {
						fscanf(in, "%d ", &help);
						img->r[i][j] = (int)help;
					}
				}
				img->is_gray = 1;
			} else {
				// colour
				for (int i = 0; i < img->m; ++i) {
					for (int j = 0; j < img->n; ++j) {
						fscanf(in, "%d ", &help);
						img->r[i][j] = (int)help;
						fscanf(in, "%d ", &help);
						img->g[i][j] = (int)help;
						fscanf(in, "%d ", &help);
						img->b[i][j] = (int)help;
						img->r[i][j] *= LHELP;
						img->g[i][j] *= LHELP;
						img->b[i][j] *= LHELP;
					}
				}
				img->is_gray = 0;
			}
			fclose(in);
		} else {
			// binary
			in = fopen(filename, "rb");
			fsetpos(in, &position1);
			if (img->type == 5) {
				// gray
				for (int i = 0; i < img->m; ++i) {
					for (int j = 0; j < img->n; ++j) {
						unsigned char aux;
						fread(&aux, sizeof(unsigned char), 1, in);
						img->r[i][j] = (int)aux;
					}
				}
				img->is_gray = 1;
			} else {
				// colour binary
				for (int i = 0; i < img->m; ++i) {
					for (int j = 0; j < img->n; ++j) {
						unsigned char aux;
						fread(&aux, sizeof(unsigned char), 1, in);
						img->r[i][j] = (int)aux;
						fread(&aux, sizeof(unsigned char), 1, in);
						img->g[i][j] = (int)aux;
						fread(&aux, sizeof(unsigned char), 1, in);
						img->b[i][j] = (int)aux;
						img->r[i][j] *= LHELP;
						img->g[i][j] *= LHELP;
						img->b[i][j] *= LHELP;
					}
				}
				img->is_gray = 0;
			}
			fclose(in);
		}
		img->exists_loaded = 1;
		printf("Loaded %s\n", filename);
		// by default selection is all image ALL
		img->x1 = 0;
		img->y1 = 0;
		img->x2 = img->n;
		img->y2 = img->m;
	}
}

void select_simple(image *img, int x1, int y1, int x2, int y2)
{
	if (img->exists_loaded == 0) {
		printf("No image loaded\n");
	} else {
		if (x1 > x2) {
			// swap to make it valid
			int aux = x1;
			x1 = x2;
			x2 = aux;
		}
		if (y1 > y2) {
			// swap in order to make it valid
			int aux = y1;
			y1 = y2;
			y2 = aux;
		}
		if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0) {
			printf("Invalid command\n");
			return;
		}
		if (x2 > img->n || y2 > img->m || x1 == x2 || y1 == y2) {
			printf("Invalid set of coordinates\n");
		} else {
			if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0) {
				printf("Invalid command\n");
				return;
			}
			img->x1 = x1;
			img->x2 = x2;
			img->y1 = y1;
			img->y2 = y2;
			printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
		}
	}
}

int clamp_func(int x)
{
	if (x > (255 * LHELP))
		return (255 *  LHELP);
	if (x < 0)
		return 0;
	return x;
}

void equalize_function(image *img)
{
	if (img->exists_loaded == 0) {
		printf("No image loaded\n");
	} else {
		if (img->is_gray == 0) {
			printf("Black and white image needed\n");
		} else {
			int *freq = malloc(256 * sizeof(int));
			for (int k = 0; k < 256; ++k)
				freq[k] = 0;

			for (int i = 0; i < img->n; ++i)
				for (int j = 0; j < img->m; ++j)
					freq[((img->r)[j][i])]++;

			int area = img->n * img->m;
			int *new_values = malloc(256 * sizeof(int));
			for (int i = 0; i < 256; ++i)
				new_values[i] = 0;
			int sum;
			for (int i = 0; i <= img->max_val; ++i) {
				sum = 0;
				for (int j = 0; j <= i; ++j)
					sum += freq[j];

				new_values[i] = ((255 * sum) / area);
			}

			for (int i = 0; i < img->n; ++i)
				for (int j = 0; j < img->m; ++j)
					img->r[j][i] = new_values[img->r[j][i]];

			printf("Equalize done\n");
			free(freq);
			free(new_values);
		}
	}
}

void select_all(image *img)
{
	if (img->exists_loaded == 0) {
		printf("No image loaded\n");
	} else {
		img->x1 = 0;
		img->y1 = 0;
		img->x2 = img->n;
		img->y2 = img->m;
		printf("Selected ALL\n");
	}
}

void save_comm(char filename[], image *img, int ascii)
{
	if (img->exists_loaded == 0) {
		printf("No image loaded\n");
		return;
	}
	if (filename[strlen(filename) - 1] == '\n')
		strcpy(filename + strlen(filename) - 1, filename + strlen(filename));

	printf("Saved %s\n", filename);
	if (ascii == 1) {
		FILE *out;
		out = fopen(filename, "wt");
		if (img->is_gray == 1) {
			fprintf(out, "P2\n");
			fprintf(out, "%d %d\n", img->n, img->m);
			fprintf(out, "%d\n", 255);
			for (int i = 0; i < img->m; ++i) {
				for (int j = 0; j < img->n; ++j)
					fprintf(out, "%d ", img->r[i][j]);

				fprintf(out, "\n");
			}
		} else {
			fprintf(out, "P3\n");
			fprintf(out, "%d %d\n", img->n, img->m);
			fprintf(out, "%d\n", 255);
			for (int i = 0; i < img->m; ++i) {
				for (int j = 0; j < img->n; ++j) {
					img->r[i][j] /= LHELP;
					img->g[i][j] /= LHELP;
					img->b[i][j] /= LHELP;
					fprintf(out, "%d ", img->r[i][j]);
					fprintf(out, "%d ", img->g[i][j]);
					fprintf(out, "%d ", img->b[i][j]);
					img->r[i][j] *= LHELP;
					img->g[i][j] *= LHELP;
					img->b[i][j] *= LHELP;
				}
				fprintf(out, "\n");
			}
		}
		fclose(out);
	} else {
		FILE *out = fopen(filename, "wt");
		if (img->is_gray == 1) {
			fprintf(out, "P5\n");
			fprintf(out, "%d %d\n", img->n, img->m);
			fprintf(out, "%d\n", img->max_val);
			fpos_t pos1;
			fgetpos(out, &pos1);
			fclose(out);
			FILE *outt = fopen(filename, "ab");
			fsetpos(outt, &pos1);
			for (int i = 0; i < img->m; ++i)
				for (int j = 0; j < img->n; ++j)
					fwrite(&img->r[i][j], sizeof(char), 1, outt);

			fclose(outt);
		} else {
			fprintf(out, "P6\n");
			fprintf(out, "%d %d\n", img->n, img->m);
			fprintf(out, "%d\n", img->max_val);
			fpos_t pos1;
			fgetpos(out, &pos1);
			fclose(out);
			FILE *outt = fopen(filename, "ab");
			fsetpos(outt, &pos1);
			for (int i = 0; i < img->m; ++i)
				for (int j = 0; j < img->n; ++j) {
					img->r[i][j] /= LHELP;
					img->g[i][j] /= LHELP;
					img->b[i][j] /= LHELP;
					fwrite(&img->r[i][j], sizeof(unsigned char), 1, outt);
					fwrite(&img->g[i][j], sizeof(unsigned char), 1, outt);
					fwrite(&img->b[i][j], sizeof(unsigned char), 1, outt);
					img->r[i][j] *= LHELP;
					img->g[i][j] *= LHELP;
					img->b[i][j] *= LHELP;
				}

			fclose(outt);
		}
	}
}

void crop_function(image *img)
{
	if (img->exists_loaded == 0) {
		printf("No image loaded\n");
	} else {
		printf("Image cropped\n");
		if (img->is_gray == 1) {
			int **aux1 = malloc((img->y2 - img->y1) * sizeof(int *));
			for (int i = 0; i < (img->y2 - img->y1); ++i)
				aux1[i] = malloc((img->x2 - img->x1) * sizeof(int));

			for (int i = 0; i < (img->y2 - img->y1); ++i) {
				for (int j = 0; j < (img->x2 - img->x1); ++j)
					aux1[i][j] = (img->r)[i + img->y1][j + img->x1];
			}
			int **sw1 = img->r;
			img->r = aux1;

			for (int i = 0; i < img->m; ++i)
				free(sw1[i]);
			free(sw1);

			img->n = img->x2 - img->x1;
			img->m = img->y2 - img->y1;
			img->x1 = 0;
			img->y1 = 0;
			img->x2 = img->n;
			img->y2 = img->m;
		} else {
			// is colour
			int **aux1 = malloc((img->y2 - img->y1) * sizeof(int *));
			for (int i = 0; i < (img->y2 - img->y1); ++i)
				aux1[i] = malloc((img->x2 - img->x1) * sizeof(int));

			for (int i = 0; i < (img->y2 - img->y1); ++i) {
				for (int j = 0; j < (img->x2 - img->x1); ++j)
					aux1[i][j] = (img->r)[i + img->y1][j + img->x1];
			}
			int **sw1 = img->r;
			img->r = aux1;

			for (int i = 0; i < img->m; ++i)
				free(sw1[i]);
			free(sw1);

			int **aux2 = malloc((img->y2 - img->y1) * sizeof(int *));
			for (int i = 0; i < (img->y2 - img->y1); ++i)
				aux2[i] = malloc((img->x2 - img->x1) * sizeof(int));

			for (int i = 0; i < (img->y2 - img->y1); ++i) {
				for (int j = 0; j < (img->x2 - img->x1); ++j)
					aux2[i][j] = (img->g)[i + img->y1][j + img->x1];
			}
			int **sw2 = img->g;
			img->g = aux2;

			for (int i = 0; i < img->m; ++i)
				free(sw2[i]);
			free(sw2);

			int **aux3 = malloc((img->y2 - img->y1) * sizeof(int *));
			for (int i = 0; i < (img->y2 - img->y1); ++i)
				aux3[i] = malloc((img->x2 - img->x1) * sizeof(int));

			for (int i = 0; i < (img->y2 - img->y1); ++i)
				for (int j = 0; j < (img->x2 - img->x1); ++j)
					aux3[i][j] = (img->b)[i + img->y1][j + img->x1];

			int **sw3 = img->b;
			img->b = aux3;

			for (int i = 0; i < img->m; ++i)
				free(sw3[i]);
			free(sw3);

			img->n = img->x2 - img->x1;
			img->m = img->y2 - img->y1;
			img->x1 = 0;
			img->y1 = 0;
			img->x2 = img->n;
			img->y2 = img->m;
		}
	}
}

void rotate_once(image *img)
{
	// function for rotating to right
	if (img->exists_loaded == 0) {
		printf("No image loaded\n");
		return;
	}
	if (img->x1 == 0 && img->x2 == img->n && img->y1 == 0 && img->y2 == img->m) {
		// all image is selected
		if (img->is_gray == 1) {
			int **aux = malloc(img->n * sizeof(int *));
			for (int i = 0; i < img->n; ++i)
				aux[i] = malloc(img->m * sizeof(int));
			// am o matrice de dimensiuni inverse
			for (int i = 0; i < img->m; ++i)
				for (int j = 0; j < img->n; ++j)
					aux[j][img->m - i - 1] = (img->r)[i][j];
			for (int i = 0; i < img->m; ++i)
				free(img->r[i]);
			free(img->r);
			img->r = aux;
			int swapy = img->n;
			img->n = img->m;
			img->m = swapy;
			swapy = img->x2;
			img->x2 = img->y2;
			img->y2 = swapy;
		} else {
			// in this case colour image
			int **aux1 = malloc(img->n * sizeof(int *));
			for (int i = 0; i < img->n; ++i)
				aux1[i] = malloc(img->m * sizeof(int));
			// am o matrice de dimensiuni inverse
			for (int i = 0; i < img->m; ++i)
				for (int j = 0; j < img->n; ++j)
					aux1[j][img->m - i - 1] = (img->r)[i][j];
			for (int i = 0; i < img->m; ++i)
				free(img->r[i]);
			free(img->r);
			img->r = aux1;

			int **aux2 = malloc(img->n * sizeof(int *));
			for (int i = 0; i < img->n; ++i)
				aux2[i] = malloc(img->m * sizeof(int));
			// am o matrice de dimensiuni inverse
			for (int i = 0; i < img->m; ++i)
				for (int j = 0; j < img->n; ++j)
					aux2[j][img->m - i - 1] = (img->g)[i][j];
			for (int i = 0; i < img->m; ++i)
				free(img->g[i]);
			free(img->g);
			img->g = aux2;

			int **aux3 = malloc(img->n * sizeof(int *));
			for (int i = 0; i < img->n; ++i)
				aux3[i] = malloc(img->m * sizeof(int));
			// am o matrice de dimensiuni inverse
			for (int i = 0; i < img->m; ++i)
				for (int j = 0; j < img->n; ++j)
					aux3[j][img->m - i - 1] = (img->b)[i][j];
			for (int i = 0; i < img->m; ++i)
				free(img->b[i]);
			free(img->b);
			img->b = aux3;
			int swapy = img->n;
			img->n = img->m;
			img->m = swapy;
			swapy = img->x2;
			img->x2 = img->y2;
			img->y2 = swapy;
		}
	} else {
		if (img->x2 - img->x1 != img->y2 - img->y1) {
			printf("The selection must be square\n");
		} else {
			if (img->is_gray == 1) {
				// gray selection
				int size = img->x2 - img->x1;
				int **helper1 = malloc(size * sizeof(int *));
				int **helper2 = malloc(size * sizeof(int *));
				for (int i = 0; i < size; ++i) {
					helper1[i] = malloc(size * sizeof(int));
					helper2[i] = malloc(size * sizeof(int));
				}
				for (int i = 0; i < size; ++i)
					for (int j = 0; j < size; ++j)
						helper1[i][j] = img->r[i + img->y1][j + img->x1];

				for (int i = 0; i < size; ++i)
					for (int j = 0; j < size; ++j)
						helper2[i][j] = helper1[size - j - 1][i];

				for (int i = 0; i < size; ++i)
					for (int j = 0; j < size; ++j)
						img->r[i + img->y1][j + img->x1] = helper2[i][j];

				for (int i = 0; i < size; ++i)
					free(helper1[i]);
				free(helper1);
				for (int i = 0; i < size; ++i)
					free(helper2[i]);
				free(helper2);
						
			} else {
				// colour selection
				int size = img->x2 - img->x1;
				int **helper1 = malloc(size * sizeof(int *));
				int **helper2 = malloc(size * sizeof(int *));
				for (int i = 0; i < size; ++i) {
					helper1[i] = malloc(size * sizeof(int));
					helper2[i] = malloc(size * sizeof(int));
				}
				for (int i = 0; i < size; ++i)
					for (int j = 0; j < size; ++j)
						helper1[i][j] = img->r[i + img->y1][j + img->x1];

				for (int i = 0; i < size; ++i)
					for (int j = 0; j < size; ++j)
						helper2[i][j] = helper1[size - j - 1][i];

				for (int i = 0; i < size; ++i)
					for (int j = 0; j < size; ++j)
						img->r[i + img->y1][j + img->x1] = helper2[i][j];

				for (int i = 0; i < size; ++i)
					for (int j = 0; j < size; ++j)
						helper1[i][j] = img->g[i + img->y1][j + img->x1];

				for (int i = 0; i < size; ++i)
					for (int j = 0; j < size; ++j)
						helper2[i][j] = helper1[size - j - 1][i];

				for (int i = 0; i < size; ++i)
					for (int j = 0; j < size; ++j)
						img->g[i + img->y1][j + img->x1] = helper2[i][j];

				for (int i = 0; i < size; ++i)
					for (int j = 0; j < size; ++j)
						helper1[i][j] = img->b[i + img->y1][j + img->x1];

				for (int i = 0; i < size; ++i)
					for (int j = 0; j < size; ++j)
						helper2[i][j] = helper1[size - j - 1][i];

				for (int i = 0; i < size; ++i)
					for (int j = 0; j < size; ++j)
						img->b[i + img->y1][j + img->x1] = helper2[i][j];

				for (int i = 0; i < size; ++i)
					free(helper1[i]);
				free(helper1);
				for (int i = 0; i < size; ++i)
					free(helper2[i]);
				free(helper2);
			}
		}
	}
}

void rotate_func(image *img, int unghi, int semn)
{
	if (img->exists_loaded == 0) {
		printf("No image loaded\n");
		return;
	}
	if (semn == 1)
		unghi = -unghi;
	if (unghi % 90 != 0) {
		printf("Unsupported rotation angle\n");
		return;
	}
	// now we can do the rotation
	printf("Rotated %d\n", unghi);
	unghi = unghi + 360 * 8;
	unghi = unghi % 360;
	if (unghi == 90) {
		rotate_once(img);
		return;
	}
	if (unghi == 180) {
		rotate_once(img);
		rotate_once(img);
		return;
	}
	if (unghi == 270) {
		rotate_once(img);
		rotate_once(img);
		rotate_once(img);
		return;
	}
}

void histogram_func(image *img, int X, int Y)
{
	if (img->exists_loaded == 0) {
		printf("No image loaded\n");
		return;
	}
	if (img->is_gray == 0) {
		printf("Black and white image needed\n");
		return;
	}
	if (!(X > 0 && Y > 0)) {
		printf("Invalid set of parameters\n");
		return;
	}

	if (!(Y == 2 || Y == 4 || Y == 8 || Y == 16 ||
		Y == 32 || Y == 64 || Y == 128 || Y == 256)) {
		printf("Invalid set of parameters\n");
		return;
	}

	// acum stam bine cu parametri
	int *freq = calloc(Y, sizeof(int));
	int help_y = 256 / Y;
	for (int i = 0; i < img->n; ++i)
		for (int j = 0; j < img->m; ++j)
			freq[(img->r[j][i]) / help_y]++;

	int max_fr = 0;
	for (int i = 0; i < Y; ++i)
		if (max_fr < freq[i])
			max_fr = freq[i];

	for (int i = 0; i < Y; ++i) {
		int help = ((freq[i] * X) / max_fr);
		printf("%d\t|\t", help);
		for (int j = 0; j < help; ++j)
			printf("*");
		printf("\n");
	}
	free(freq);
}

void apply_kernel(image *img, char filter_name[])
{
	if (img->exists_loaded == 0) {
		printf("No image loaded\n");
		return;
	}
	if (img->is_gray == 1) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	if (filter_name[strlen(filter_name) - 1] == '\n')
		strcpy(filter_name + strlen(filter_name) - 1,
				filter_name + strlen(filter_name));
	int ok = 0;
	double ker[3][3];
	if (strncmp(filter_name, "EDGE", 5) == 0) {
		ok = 1;
		ker[0][0] = -1, ker[0][1] = -1, ker[0][2] = -1;
		ker[1][0] = -1, ker[1][1] = 8, ker[1][2] = -1;
		ker[2][0] = -1, ker[2][1] = -1, ker[2][2] = -1;
	}
	if (strncmp(filter_name, "SHARPEN", 7) == 0) {
		ok = 1;
		ker[0][0] = 0, ker[0][1] = -1, ker[0][2] = 0;
		ker[1][0] = -1, ker[1][1] = 5, ker[1][2] = -1;
		ker[2][0] = 0, ker[2][1] = -1, ker[2][2] = 0;
	}
	if (strncmp(filter_name, "BLUR", 5) == 0) {
		ok = 1;
		ker[0][0] = 1.0 / 9, ker[0][1] = 1.0 / 9, ker[0][2] = 1.0 / 9;
		ker[1][0] = 1.0 / 9, ker[1][1] = 1.0 / 9, ker[1][2] = 1.0 / 9;
		ker[2][0] = 1.0 / 9, ker[2][1] = 1.0 / 9, ker[2][2] = 1.0 / 9;
	}
	if (strncmp(filter_name, "GAUSSIAN_BLUR", 13) == 0) {
		ok = 1;
		ker[0][0] = 1.0 / 16, ker[0][1] = 1.0 / 8, ker[0][2] = 1.0 / 16;
		ker[1][0] = 1.0 / 8, ker[1][1] = 1.0 / 4, ker[1][2] = 1.0 / 8;
		ker[2][0] = 1.0 / 16, ker[2][1] = 1.0 / 8, ker[2][2] = 1.0 / 16;
	}
	if (ok == 0) {
		printf("APPLY parameter invalid\n");
		return;
	}
	int y_ngh[8] = {-1, -1, -1, 0, 1, 1,  1,  0};
	int x_ngh[8] = {-1,  0,  1, 1, 1, 0, -1, -1};
	int **col = malloc(img->m * sizeof(int *));
	for (int i = 0; i < img->m; ++i)
		col[i] = malloc(img->n * sizeof(int));

	for (int i = 0; i < img->m; ++i)
		for (int j = 0; j < img->n; ++j)
			col[i][j] = img->r[i][j];

	for (int i = (img->y1); i < (img->y2); ++i)
		for (int j = (img->x1); j < (img->x2); ++j) {
			if ((i != 0 && j != 0 && i != (img->m - 1) && j != (img->n - 1))) {
				double auxi = col[i][j] * ker[1][1];
				for (int k = 0; k < 8; ++k)
					auxi += 1.0 * col[i + y_ngh[k]][j + x_ngh[k]] * ker[1 + y_ngh[k]][1 + x_ngh[k]];
				int pix = clamp_func((int)auxi);
				img->r[i][j] = pix;
			}
		}

	for (int i = 0; i < img->m; ++i)
		for (int j = 0; j < img->n; ++j)
			col[i][j] = img->g[i][j];

	for (int i = (img->y1); i < (img->y2); ++i)
		for (int j = (img->x1); j < (img->x2); ++j)
			if ((i != 0 && j != 0 && i != (img->m - 1) && j != (img->n - 1))) {
				double auxi = col[i][j] * ker[1][1];
				for (int k = 0; k < 8; ++k)
					auxi += 1.0 * col[i + y_ngh[k]][j + x_ngh[k]] * ker[1 + y_ngh[k]][1 + x_ngh[k]];
				int pix = clamp_func((int)auxi);
				img->g[i][j] = pix;
			}

	for (int i = 0; i < img->m; ++i)
		for (int j = 0; j < img->n; ++j)
			col[i][j] = img->b[i][j];

	for (int i = (img->y1); i < (img->y2); ++i)
		for (int j = (img->x1); j < (img->x2); ++j)
			if ((i != 0 && j != 0 && i != (img->m - 1) && j != (img->n - 1))) {
				double auxi = col[i][j] * ker[1][1];
				for (int k = 0; k < 8; ++k)
					auxi += 1.0 * col[i + y_ngh[k]][j + x_ngh[k]] * ker[1 + y_ngh[k]][1 + x_ngh[k]];
				int pix = clamp_func((int)auxi);
				img->b[i][j] = pix;
			}

	for (int i = 0; i < img->m; ++i)
		free(col[i]);
	free(col);
	printf("APPLY %s done\n", filter_name);
}

int main(void)
{
	image img;
	img.exists_loaded = 0;
	img.type = 255;
	char buffer[LBUFF];
	fgets(buffer, LBUFF, stdin);
	char *p = strtok(buffer, " ");
	int valid = 0;
	while (strncmp(p, "EXIT", 4) != 0) {
		if (p && strcmp(p, "LOAD") == 0) {
			p = strtok(NULL, " ");
			load_function(p, &img);
			valid = 1;
		}

		if (p && strcmp(p, "SELECT") == 0) {
			int minus = 0;
			p = strtok(NULL, " ");
			if (strcmp(p, "ALL\n") == 0 || strcmp(p, "ALL") == 0) {
				select_all(&img);
				valid = 1;
			} else {
				if (p[0] == '-')
					minus++;
				int x1 = atoi(p);
				p = strtok(NULL, " ");
				if (p) {
					if (p[0] == '-')
						minus++;
					int y1 = atoi(p);
					p = strtok(NULL, " ");
					if (p) {
						if (p[0] == '-')
							minus++;
						int x2 = atoi(p);
						p = strtok(NULL, " ");
						if (p) {
							if (p[0] == '-')
								minus++;
							int y2 = atoi(p);
							if (minus == 0) {
								select_simple(&img, x1, y1, x2, y2);
								valid = 1;
							} else {
								if (img.exists_loaded == 1)
									printf("Invalid set of coordinates\n");
								else
									printf("No image loaded\n");
								valid = 1;
							}
						} else {
							printf("Invalid command\n");
						}
					} else {
					printf("Invalid command\n");
					}
				} else {
					printf("Invalid command\n");
				}
			}
		}

		if (p && strcmp(p, "ROTATE") == 0) {
			p = strtok(NULL, " ");
			int unghi;
			if (p) {
				int signum = 0;
				if (p[0] == '-') {
					signum = 1;
					unghi = atoi(p + 1);
				} else {
					unghi = atoi(p);
				}
				rotate_func(&img, unghi, signum);
				valid = 1;
			}
		}

		if (p && (strcmp(p, "EQUALIZE\n") == 0 || strcmp(p, "EQUALIZE") == 0)) {
			equalize_function(&img);
			valid = 1;
		}

		if (p && (strcmp(p, "CROP") == 0 || strcmp(p, "CROP\n") == 0)) {
			crop_function(&img);
			valid = 1;
		}

		if (p && strncmp(p, "APPLY", 5) == 0) {
			p = strtok(NULL, " ");
			if (!p && img.exists_loaded == 0) {
				printf("No image loaded\n");
				valid = 1;
			}
			if (!p && img.exists_loaded == 1) {
				printf("Invalid command\n");
				valid = 1;
			}
			if (p && strcmp(p, "\n") != 0) {
				apply_kernel(&img, p);
				valid = 1;
			}
		}

		if (p && strcmp(p, "HISTOGRAM") == 0) {
			p = strtok(NULL, " ");
			if (p) {
			int X = atoi(p);
			p = strtok(NULL, " ");
				if (p) {
					int Y = atoi(p);
					p = strtok(NULL, " ");
					if (!p) {
						histogram_func(&img, X, Y);
						valid = 1;
					}
				}
			}
		}

		if (p && strcmp(p, "SAVE") == 0) {
			p = strtok(NULL, " ");
			char *fisier;
			fisier = p;
			int ascii = 0;
			valid = 1;
			p = strtok(NULL, " ");
			if (p && strncmp(p, "ascii", 5) == 0) {
				ascii = 1;
				valid = 1;
				p = strtok(NULL, " ");
			}
			save_comm(fisier, &img, ascii);
		}

		if (valid == 0) {
			if (p && strcmp(p, "HISTOGRAM\n") == 0 && img.exists_loaded == 0)
				printf("No image loaded\n");
			else
				printf("Invalid command\n");
		}
		valid = 0;
		fgets(buffer, LBUFF, stdin);
		p = strtok(buffer, " ");
	}

	if (img.exists_loaded == 0)
		printf("No image loaded\n");

	// now free
	free_img(&img);
	img.exists_loaded = 0;
	return 0;
}
