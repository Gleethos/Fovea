// Based on public domain code by Antonio R. Fernandes <ajbrf@yahoo.com>

#ifdef _WIN32
#include <windows.h>
#endif


#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else 
#include <GL/glu.h>
#include <GL/glut.h>
#endif


#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <stdint.h>
#endif


#include "tga.h"

// this variable is used for image series
static int savedImages=0;

#ifndef _WIN32 
static void
swap_16bit_word(uint16_t *word)
{
  uint8_t *block = (uint8_t *)word;
  uint8_t tmp;
  tmp = block[1];
  block[1] = block[0];
  block[0] = tmp;
}
#endif

// load the image header fields. We only keep those that matter!
void tgaLoadHeader(FILE *file, tgaInfo *info) {

	unsigned char cGarbage;
	short int iGarbage;

	fread(&cGarbage, sizeof(unsigned char), 1, file);
	fread(&cGarbage, sizeof(unsigned char), 1, file);

// type must be 2 or 3
	fread(&info->type, sizeof(unsigned char), 1, file);

	fread(&iGarbage, sizeof(short int), 1, file);
	fread(&iGarbage, sizeof(short int), 1, file);
	fread(&cGarbage, sizeof(unsigned char), 1, file);
	fread(&iGarbage, sizeof(short int), 1, file);
	fread(&iGarbage, sizeof(short int), 1, file);

	fread(&info->width, sizeof(short int), 1, file);
	fread(&info->height, sizeof(short int), 1, file);
	fread(&info->pixelDepth, sizeof(unsigned char), 1, file);

#ifdef __APPLE__
  // FIXME: Determine byte order at runtime.
  swap_16bit_word((uint16_t*)&info->width);
  swap_16bit_word((uint16_t*)&info->height);
#endif // __APPLE__

#if 0 // debug
  printf("Image dimensions: %d %d %d\n", 
         info->width, info->height, info->pixelDepth);
#endif

	fread(&cGarbage, sizeof(unsigned char), 1, file);
}

// loads the image pixels. You shouldn't call this function
// directly
void tgaLoadImageData(FILE *file, tgaInfo *info) {

	int mode,total,i;
	unsigned char aux;

// mode equal the number of components for each pixel
	mode = info->pixelDepth / 8;
// total is the number of bytes we'll have to read
	total = info->height * info->width * mode;
	
	fread(info->imageData,sizeof(unsigned char),total,file);

#if 0 // debug
  printf("Image mode: %d\n", mode);
#endif

// mode=3 or 4 implies that the image is RGB(A). However TGA
// stores it as BGR(A) so we'll have to swap R and B.
	if (mode >= 3)
		for (i=0; i < total; i+= mode) {
#if 1 // original code
			aux = info->imageData[i];
			info->imageData[i] = info->imageData[i+2];
			info->imageData[i+2] = aux;
#else // somehow I had a texture file which was RBG (!) 
			aux = info->imageData[i+1];
			info->imageData[i+1] = info->imageData[i+2];
			info->imageData[i+2] = aux;
#endif
		}
}	

// this is the function to call when we want to load
// an image
tgaInfo * tgaLoad(char *filename) {
	
	FILE *file;
	tgaInfo *info;
	int mode,total;

// allocate memory for the info struct and check!
	info = (tgaInfo *)malloc(sizeof(tgaInfo));
	if (info == NULL)
		return(NULL);


// open the file for reading (binary mode)
	file = fopen(filename, "rb");
	if (file == NULL) {
		info->status = TGA_ERROR_FILE_OPEN;
		return(info);
	}

  clearerr(file);

// load the header
	tgaLoadHeader(file,info);

// check for errors when loading the header
	if (ferror(file)) {
		info->status = TGA_ERROR_READING_FILE;
    fprintf(stderr, "error reading file\n");
		fclose(file);
		return(info);
	}

// check if the image is color indexed
	if (info->type == 1) {
		info->status = TGA_ERROR_INDEXED_COLOR;
		fclose(file);
		return(info);
	}

#if 0
// check for other types (compressed images)
	if ((info->type != 2) && (info->type !=3)) {
		info->status = TGA_ERROR_COMPRESSED_FILE;
    fprintf(stderr, "error: compressed file (type was %d)\n", info->type);
		fclose(file);
		return(info);
	}
#endif

// mode equals the number of image components
	mode = info->pixelDepth / 8;
// total is the number of bytes to read
	total = info->height * info->width * mode;
// allocate memory for image pixels
	info->imageData = (unsigned char *)malloc(sizeof(unsigned char) * 
															total);

// check to make sure we have the memory required
	if (info->imageData == NULL) {
		info->status = TGA_ERROR_MEMORY;
		fclose(file);
		return(info);
	}
// finally load the image pixels
	tgaLoadImageData(file,info);

// check for errors when reading the pixels
	if (ferror(file)) {
		info->status = TGA_ERROR_READING_FILE;
		fclose(file);
		return(info);
	}
	fclose(file);
	info->status = TGA_OK;
	return(info);
}		

// converts RGB to greyscale
void tgaRGBtoGreyscale(tgaInfo *info) {

	int mode,i,j;

	unsigned char *newImageData;

// if the image is already greyscale do nothing
	if (info->pixelDepth == 8)
		return;

// compute the number of actual components
	mode = info->pixelDepth / 8;

// allocate an array for the new image data
	newImageData = (unsigned char *)malloc(sizeof(unsigned char) * 
											info->height * info->width);
	if (newImageData == NULL) {
		return;
	}

// convert pixels: greyscale = o.30 * R + 0.59 * G + 0.11 * B
	for (i = 0,j = 0; j < info->width * info->height; i +=mode, j++)
		newImageData[j] =	(unsigned char)(0.30 * info->imageData[i] + 
						0.59 * info->imageData[i+1] +
						0.11 * info->imageData[i+2]);


//free old image data
	free(info->imageData);

// reassign pixelDepth and type according to the new image type
	info->pixelDepth = 8;
	info->type = 3;
// reassing imageData to the new array.
	info->imageData = newImageData;
}

// takes a screen shot and saves it to a TGA image
int tgaGrabScreenSeries(char *filename, int x,int y, int w, int h) {
	
	unsigned char *imageData;

// allocate memory for the pixels
	imageData = (unsigned char *)malloc(sizeof(unsigned char) * w * h * 4);

// read the pixels from the frame buffer
	glReadPixels(x,y,w,h,GL_RGBA,GL_UNSIGNED_BYTE, (GLvoid *)imageData);

// save the image 
	return(tgaSaveSeries(filename,w,h,32,imageData));
}

// saves an array of pixels as a TGA image
int tgaSave(char			*filename, 
			 short int		width, 
			 short int		height, 
			 unsigned char	pixelDepth,
			 unsigned char	*imageData) {

	unsigned char cGarbage = 0, type,mode,aux;
	short int iGarbage = 0;
	int i;
	FILE *file;

// open file and check for errors
	file = fopen(filename, "wb");
	if (file == NULL) {
		return(TGA_ERROR_FILE_OPEN);
	}

// compute image type: 2 for RGB(A), 3 for greyscale
	mode = pixelDepth / 8;
	if ((pixelDepth == 24) || (pixelDepth == 32))
		type = 2;
	else
		type = 3;

// write the header
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);

	fwrite(&type, sizeof(unsigned char), 1, file);

	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);

	fwrite(&width, sizeof(short int), 1, file);
	fwrite(&height, sizeof(short int), 1, file);
	fwrite(&pixelDepth, sizeof(unsigned char), 1, file);

	fwrite(&cGarbage, sizeof(unsigned char), 1, file);

// convert the image data from RGB(a) to BGR(A)
	if (mode >= 3)
	for (i=0; i < width * height * mode ; i+= mode) {
		aux = imageData[i];
		imageData[i] = imageData[i+2];
		imageData[i+2] = aux;
	}

// save the image data
	fwrite(imageData, sizeof(unsigned char), width * height * mode, file);
	fclose(file);
// release the memory
	free(imageData);

	return(TGA_OK);
}

// saves a series of files with names "filenameX.tga"
int tgaSaveSeries(char		*filename, 
			 short int		width, 
			 short int		height, 
			 unsigned char	pixelDepth,
			 unsigned char	*imageData) {
	
	char *newFilename;
	int status;
// compute the new filename by adding the series number and the extension

	newFilename = (char *)malloc(sizeof(char) * strlen(filename)+8);

	sprintf(newFilename,"%s%d.tga",filename,savedImages);
// save the image
	status = tgaSave(newFilename,width,height,pixelDepth,imageData);
//increase the counter
	savedImages++;
	return(status);
}


// releases the memory used for the image
void tgaDestroy(tgaInfo *info) {

	if (info != NULL) {
		free(info->imageData);
		free(info);
	}
}
