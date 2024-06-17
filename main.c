/* ************************************************************************************************ */
/*                                                                                                  */
/*                                                        :::   ::::::::   ::::::::  :::::::::::    */
/*   main.c                                            :+:+:  :+:    :+: :+:    :+: :+:     :+:     */
/*                                                      +:+         +:+        +:+        +:+       */
/*   By: aym3n <azainabi@student.1337.ma>              +#+      +#++:      +#++:        +#+         */
/*                                                    +#+         +#+        +#+      +#+           */
/*   Created: 2024/06/17 04:07:00 by aym3n           #+#  #+#    #+# #+#    #+#     #+#             */
/*   Updated: 2024/06/17 04:07:00 by aym3n        ####### ########   ########      ###.ma           */
/*                                                                                                  */
/* ************************************************************************************************ */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>

#define WIDTH 800
#define HEIGHT 600

#define WHITE 0xFFFFFFFF
#define RED   0xFF0000FF
#define GREEN 0xFF00FF00
#define BLUE  0xFFFF0000
#define BLACK 0x00000000

#define ZOOM 1.0
#define ITERATIONS 240

#define FRAMES 60 

#define START_REAL -0.8
#define START_IMAGINARY 0.7885
#define END_REAL 0.285
#define END_IMAGINARY 0.01

static int image[HEIGHT][WIDTH]; // Image buffer

typedef struct s_complex {
	double	r;
	double	i;
	double	tmp;
}				t_complex;

int ft_strlen(const char *str)
{
	int i = 0;
	while (str[i])
	{
		i++;
	}
	return (i);
}

static int	size(int n)
{
	int			count;
	long long	nl;

	nl = n;
	count = 0;
	if (nl == 0)
		count = 1;
	if (nl < 0)
	{
		nl *= -1;
		count += 1;
	}
	while (nl)
	{
		nl /= 10;
		count += 1;
	}
	return (count);
}

static void	convert(char *ptr, int length, long long nl)
{
	while (nl)
	{
		ptr[length - 1] = (nl % 10) + '0';
		nl /= 10;
		length --;
	}
}

char	*ft_itoa(int n)
{
	char		*ptr;
	long long	nl;
	int			length;

	nl = n;
	length = size(n);
	ptr = malloc(sizeof(char) * (length + 1));
	if (!ptr)
		return (NULL);
	if (nl < 0)
	{
		ptr[0] = '-';
		nl *= -1;
		convert(ptr, length, nl);
	}
	if (nl == 0)
		ptr[0] = '0';
	ptr[length] = '\0';
	if (nl >= 0)
	{
		convert(ptr, length, nl);
	}
	return (ptr);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	i;
	size_t	j;
	char	*ptr;

	i = 0;
	j = 0;
	if (!s1)
		return (NULL);
	ptr = malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (!ptr)
		return (NULL);
	while (i < ft_strlen(s1))
	{
		ptr[i] = s1[i];
		i++;
	}
	while (i < (ft_strlen(s1) + ft_strlen(s2)))
	{
		ptr[i] = s2[j];
		i++;
		j++;
	}
	ptr[i] = '\0';
	return (ptr);
}

double	interp(double value, double new_min, double new_max,
	double old_max)
{
	return (new_min + ((new_max - new_min) * (value) / (old_max)));
}

void	draw_julia(int frame)
{
	t_complex	z, c;
	double t = (double)frame / (FRAMES - 1);
    double real_julia = START_REAL * (1.0 - t) + END_REAL * t;
    double imaginary_julia = START_IMAGINARY * (1.0 - t) + END_IMAGINARY * t;
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			z.r = (interp(x, -2, 2, WIDTH) * ZOOM);
			z.i = (interp(y, -2, 2, HEIGHT) *ZOOM);
			c.r = real_julia;
			c.i = imaginary_julia;
			z.tmp = 0;
			int j = 0;
			for (j = 0; j < ITERATIONS && z.r * z.r + z.i * z.i <= 4; j++) {
				z.tmp = z.r * z.r - z.i * z.i + c.r;
				z.i = 2 * z.r * z.i + c.i;
				z.r = z.tmp;
			}
			if (j == ITERATIONS) {
				image[y][x] = BLACK;
			} else {
				uint32_t color = interp((double)j, 0x000000, 0xFFFFFF, ITERATIONS);
            	image[y][x] = color;
			}
		}
	}
}

void	fill_image(void)
{
	uint32_t color = GREEN;
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			image[y][x] = color;
		}
	}
}

void	save_image(char *path)
{
	FILE *f = fopen(path, "wb");
	if (!f) {
		perror("fopen");
		exit(13);
	}
	fprintf(f, "P6\n%d %d 255\n", WIDTH, HEIGHT);
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			int pixel = image[y][x];
			uint8_t bytes[3] = {
                (pixel & 0x00FF0000) >> 16,  
                (pixel & 0x0000FF00) >> 8,   
                (pixel & 0x000000FF)         
            };
			fwrite(bytes, sizeof(bytes), 1, f);
		}
	}
	fclose(f);
}


int main(void)
{
	char *cwd = malloc(PATH_MAX);
	if (!cwd) {
		perror("Malloc");
		exit(1);
	}
	getcwd(cwd, PATH_MAX);
	char *path_dir = ft_strjoin(cwd, "/img");
	mkdir(path_dir, 0777);
	for (int frame = 0; frame < 60; frame ++) {
		char *tmp = ft_strjoin("./img/output-", ft_itoa(frame));
		char *path = ft_strjoin(tmp, ".ppm");
		printf("path : %s, frame : %d\n", path, frame);
		draw_julia(frame);
		save_image(path);
		free(tmp);
		free(path);
	}
	printf("Generating the gif\n");
	if (execlp("/opt/homebrew/bin/ffmpeg", "ffmpeg", "-i", "./img/output-%d.ppm", "output.gif", NULL) == -1) {
		perror("execlp");
		exit(12);
	}
	printf("Gif is generated\n");
	return 0;
}