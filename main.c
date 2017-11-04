#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

unsigned char* read_bmp(char* filename, int *width, int *height, int *size, int *padding) {
    int i, j;
    FILE* f = fopen(filename, "rb");

    if(f == NULL) {
    	fprintf(stderr,"Image read failed\n");
    	return;
    }

    unsigned char info[54];
    //citeste headerul imagine BMP - 54bytes
    fread(info, sizeof(unsigned char), 54, f);

    // extract image height and width from header
    *width = *(int*)&info[18];
    *height = *(int*)&info[22];

    printf("\n");
    printf("Name: %s\n",filename);
    printf("Width: %d\n", *width);
    printf("Height: %d\n", *height);

    // Padding bytes (not necessarily 0) must 
    // be appended to the end of the rows in 
    // order to bring up the length of the rows 
    // to a multiple of four bytes. When the pixel
    // array is loaded into memory, each row must
    // begin at a memory address that is a multiple of 4.
    *padding = 0; 
    while ((( (*width) * 3 + (*padding)) % 4) != 0) {
    	(*padding)++;
    }
    printf("padding inainte: %d\n", *padding);

 	*size = ((*width) * 3 + (*padding))  * (*height);
 	printf("size: %d\n", *size);

    unsigned char* data = malloc (*size);
    fread(data, sizeof(unsigned char), *size, f);

    // unsigned char tmp;
	//in BMP pixel data is BGR position !!!
    // for(i = 0; i < (*height); i++) {
    // 	for(j = 0; j < (*width)*3; j += 3) {
    //         // Convert (B, G, R) to (R, G, B)
    //         tmp = data[j];
    //         data[j] = data[j+2];
    //         data[j+2] = tmp;
    //     }
    // }

    fclose(f);

    return data;
}

void write_bmp (unsigned char* image, int  size, char *inputFilename, char *outputFilename) {
	int i, j;
	FILE* f;
	unsigned char info[54];

	f = fopen(inputFilename, "rb");
	if(f == NULL) {
    	fprintf(stderr,"Image read failed\n");
    	return;
    }

    //citeste headerul imagine BMP - 54bytes
    fread(info, sizeof(unsigned char), 54, f);
    fclose(f);
	
	f = fopen(outputFilename, "wb");

   	fwrite (info, sizeof(unsigned char), 54, f);
	fwrite (image, sizeof(unsigned char), size, f);

	free(image);
	fclose(f);
}

/* Genereaza numele fisierului de iesire */
unsigned char* generate_output_filename(char* filter, char* input_filename) {
	unsigned char *output_filename = strdup(filter);
	strcat(output_filename, "_out_");
	strcat(output_filename, input_filename);

	return output_filename;
}

int main(int argc, char **argv) {
	int width, height, i, j, size, padding, new_width, k, resized_width, resized_height, l, m, resized_padding, o;
	float intensity;
	char *input_filename, *filter, *output_filename;
	unsigned char *input_img;

	if (argc < 2) {
		printf("Please provide 2 arguments: image_name and filter you want to apply on the image!\n");
		return;
	}

	if (input_filename == NULL || filter == NULL) {
		printf("Invalid arguments!\n");
	}

	for(k = 0; k < argc - 2; k++) {

		input_filename = argv[1];
		filter = argv[k + 2];
		input_img = read_bmp(input_filename, &width, &height, &size, &padding);
		output_filename = generate_output_filename(filter, input_filename);
		new_width = width * 3  + padding;

		printf("filter: %s\n", filter);

		if(strcmp("grayscale", filter) == 0) {

			for (i = 0; i < height; i++) {
				for (j = 0; j < new_width; j += 3) {
					intensity = (input_img[i * new_width + j] +  input_img[i * new_width + j + 1] + input_img[i * new_width + j + 2])/3;
					input_img[(i * new_width) + j] = intensity;
					input_img[(i * new_width) + (j + 1)] = intensity;
					input_img[(i * new_width) + (j + 2)] = intensity;	
				}
			}

		} else if(strcmp("invert", filter) == 0) {
			for (i = 0; i < height; i++) {
				for (j = 0; j < new_width; j += 3) {
					input_img[(i * new_width) + j] = 255 - input_img[(i * new_width) + j];
					input_img[(i * new_width) + (j + 1)] = 255 - input_img[(i * new_width) + (j + 1)];
					input_img[(i * new_width) + (j + 2)] = 255 - input_img[(i * new_width) + (j + 2)];
				}
			}

		} else if(strcmp("bluefilter", filter) == 0) {
			for (i = 0; i < height; i++) {
				for (j = 0; j < new_width; j += 3) {
					input_img[(i * new_width) + j] -= 255; 
					input_img[(i * new_width) + (j + 2)] -= 255;

					input_img[(i * new_width) + j] = max(input_img[(i * new_width) + j], 0);
	                input_img[(i * new_width) + j] = min(255, input_img[(i * new_width) + j]);

					input_img[(i * new_width) + (j + 1)] = max(input_img[(i * new_width) + (j + 1)], 0);
	                input_img[(i * new_width) + (j + 1)] = min(255, input_img[(i * new_width) + (j + 1)]);

					input_img[(i * new_width) + (j + 2)] = max(input_img[(i * new_width) + (j + 2)], 0);
	                input_img[(i * new_width) + (j + 2)] = min(255, input_img[(i * new_width) + (j + 2)]);
				}
			}

		} else if(strcmp("resize", filter) == 0) {
			int multiply_factor = atoi(argv[k + 2 + 1]);
			k++;
			resized_width = width * multiply_factor;
			resized_height = height * multiply_factor;
			printf("resized_width %d\n", resized_width);
			printf("resized_height %d\n", resized_height);

			resized_padding = 0; 
		    while (((resized_width + resized_padding) % 4) != 0) {
		    	resized_padding++;
		    }
			printf("resized_padding %d\n", resized_padding);


			int out_padding = (4 - (resized_width * 3 * sizeof(unsigned char)) % 4) % 4;
			printf("out_padding %d\n", out_padding);
			
			//DE RESCRIS
			FILE* f;
			unsigned char info[54];

			f = fopen(input_filename, "rb");
			if(f == NULL) {
		    	fprintf(stderr,"Image read failed\n");
		    	return;
		    }
		   
		    //citeste headerul imagine BMP - 54bytes
		    fread(info, sizeof(unsigned char), 54, f);
		    fclose(f);

   			*(int*)&info[18] = resized_width;
			*(int*)&info[22] = resized_height;
			
		    int biOffBits = *(int*)&info[10];
		    printf("biBitCount %d\n", biOffBits);
		    // bfSize = biSizeImage + 54      // 2          
		    // biSizeImage = (biWidth * sizeof(RGBTRIPLE) + padding) * abs(biHeight)  // 34
   		    printf("bfsize inainte %d\n", *(int*)&info[2]);
   		    printf("biSizeImage inainte %d\n", *(int*)&info[34]);
		    // printf("bfsize dupa %d\n", *(int*)&info[2]);
   		    printf("bioffbits %d\n", *(int*)&info[10]);
			// biSizeImage = (biWidth * sizeof(RGBTRIPLE) + padding) * abs(biHeight) 
		    int biSizeImage = (resized_width * 3 + resized_padding) * resized_height;
		    *(int*)&info[34] = biSizeImage;
			*(int*)&info[2] = biSizeImage + biOffBits; //resized_width * resized_height + resized_padding * resized_height;
		    printf("bfsize dupa %d\n", *(int*)&info[2]);
		    printf("biSizeImage dupa %d\n", *(int*)&info[34]);


			printf("*(int*)&info[34] biSizeImage %d\n", *(int*)&info[34]);

			f = fopen(output_filename, "wb");

		   	fwrite (info, sizeof(unsigned char), 54, f);

			for (i = 0; i < height; i++) {
				//scrie fiecare linie de multiply_factor ori
				for (l = 0; l < multiply_factor; l++) {
					//iterezi prin pixelii dintr o linie
					for (j = 0; j < new_width; j += 3) {
						//scrii pixelii in fisierul de output
						for (m = 0; m < multiply_factor; m++) {
							fwrite(&input_img[(i * new_width) + j], sizeof(unsigned char), 1, f);
							fwrite(&input_img[(i * new_width) + j + 1], sizeof(unsigned char), 1, f);
							fwrite(&input_img[(i * new_width) + j + 2], sizeof(unsigned char), 1, f);
						}
					}
					// fseek(f, padding, SEEK_CUR);
					// adaugi paddingul in fisierul nou
					for (o = 0; o < out_padding; o++)
						fputc(0x00, f);

					fseek(f, -new_width, SEEK_CUR);
				}
				fseek(f, new_width, SEEK_CUR);
			}
			free(input_img);
			fclose(f);
		}

		if (strcmp("resize", filter) != 0)
			write_bmp(input_img, size, input_filename, output_filename);
	}

	return 0;
}
