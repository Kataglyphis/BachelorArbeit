#include "stdafx.h"

#include "helpers.h"
#include <stdint.h>

using namespace std;

helpers::helpers() : blueNoiseBitMap(), dither_width(64), dither_height(64) {

}


bool helpers::freeImageFunction() {
	using namespace std;

	FreeImage_Initialise();
	
	FIBITMAP* bitmap = FreeImage_Allocate(64, 64, 16);
	if (!bitmap) exit(1);
	RGBQUAD color;
	for (int i = 0; i < 64;i++) {
		for (int j = 0; j < 64;j++) {
			color.rgbRed = 0;
			color.rgbGreen = (BYTE)(i / 64 * 255.0);
			color.rgbBlue = (BYTE)(j / 64 * 255.0);
			FreeImage_SetPixelColor(bitmap, i, j, &color);
		}
	}
	FreeImage_Save(FIF_PNG, bitmap, "test.png", 0);

	FreeImage_DeInitialise();

	return true;
}


bool helpers::loadImageFromFile() {
	blueNoiseBitMap = FreeImage_Load(FIF_PNG ,blueNoiseFile,0);

	return true;
}

bool helpers::deepCopyImage(Image& source, Image& dest, const int image_width, const int image_height) {

	for (int i = 0; i < image_width; i++) {
		for (int j = 0; j < image_height; j++) {

			int x_source = source[i][j][0];
			int y_source = source[i][j][1];
			dest[i][j][0] = x_source;
			dest[i][j][1] = y_source;
		}
	}
	return true;

}

bool helpers::fromPermuteToBitmap(Image image_data, FIBITMAP* bitmap, const uint32_t image_width, const uint32_t image_height) {

	RGBQUAD color;

	bool bitmap_successful_initialized = true;

	for (unsigned int i = 0; i < image_width; i++) {

		for (unsigned int j = 0; j < image_height; j++) {

			color.rgbRed = image_data[i][j][0] + 6;
			color.rgbGreen = image_data[i][j][1] + 6;
			//to store permutation we will only need to save in rg - channel !!
			color.rgbBlue = 0xFF;
			color.rgbReserved = i;

			if (FreeImage_SetPixelColor(bitmap, i, j, &color) == 0) {

				std::cout << "Wasnt able to set Pixel Color!!";
				bitmap_successful_initialized = false;
			}
		}
	}

	return bitmap_successful_initialized;

}

bool helpers::fromImageToBitmap(Image& image_data, FIBITMAP* bitmap, const uint32_t image_width, const uint32_t image_height) {

	RGBQUAD color;

	bool bitmap_successful_initialized = true;

	for (unsigned int i = 0; i < image_width; i++) {

		for (unsigned int j = 0; j < image_height; j++) {

			color.rgbRed = image_data[i][image_height - 1 - j][0];
			color.rgbGreen = image_data[i][image_height - 1 - j][1];
			//to store permutation we will only need to save in rg - channel !!
			color.rgbBlue = image_data[i][image_height - 1 - j][2];
			color.rgbReserved = image_data[i][image_height - 1 - j][3];

			// u really need t oconsider the order free image is loading the pixels!!!
			//free image is indexing an image from the bottom left as (0,0); 
			//therefore "invert" image
			if (FreeImage_SetPixelColor(bitmap, i, j, &color) == 0) {
				std::cout << "Wasnt able to set Pixel Color!!";
				bitmap_successful_initialized = false;
			}
		}
	}

	return bitmap_successful_initialized;

}

bool helpers::getNextDither(Image dither_data, Image& next_dither_data, const uint32_t frame_width, const uint32_t frame_height) {

	for (unsigned int i = 0; i < frame_width; i++) {
		for (unsigned int j = 0; j < frame_height; j++) {

			std::vector<int> new_dither_positions = toroidallyShift(i, j, frame_width, frame_height);
			int new_dither_x = new_dither_positions[0];
			int new_dither_y = new_dither_positions[1];
			next_dither_data[new_dither_x][new_dither_y] = dither_data[i][j];
		}
	}

	return true;

}

bool helpers::saveImageToFile(const char* filenameToSave, FIBITMAP* retargetBitMap) {

	return FreeImage_Save(FIF_PNG, retargetBitMap, filenameToSave, PNG_Z_NO_COMPRESSION);

}

bool helpers::loadPNGinArray(const char* fileName, Image& image_data) {

	FIBITMAP* bitmap = FreeImage_Load(FIF_PNG, fileName, PNG_DEFAULT);

	if (!bitmap) std::cout << "bitmap konnte nicht erstellt werden!!";

	int image_height = FreeImage_GetHeight(bitmap);
	int image_width = FreeImage_GetWidth(bitmap);

	bool catched_all_pixel_colors = true;

	//allocate store in appropriate size
	RGBQUAD color;
	for (int i = 0; i < image_width; i++) {

		for (int j = 0; j < image_height; j++) {

			// u really need t oconsider the order free image is loading the pixels!!!
			//free image is indexing an image from the bottom left as (0,0); 
			//therefore "invert" image
			if (FreeImage_GetPixelColor(bitmap, i, j, &color) == 0) {
				std::cout << "Was not able to get the pixel color!";
				catched_all_pixel_colors = false;
			}

			image_data[i][image_height - 1 - j][0] = color.rgbRed;
			image_data[i][image_height - 1 - j][1] = color.rgbGreen;
			image_data[i][image_height - 1 - j][2] = color.rgbBlue;
			image_data[i][image_height - 1 - j][3] = color.rgbReserved;

		}

	}

	return catched_all_pixel_colors;
}

std::vector<int> helpers::toroidallyShift(const unsigned int oldFrameDitherX, const unsigned int oldFrameDitherY, const uint32_t frame_width, const uint32_t frame_height) {

	std::vector<int> new_positions(2, 0);
	using namespace std;
	double g = 1.32471795724474602596;
	double a1 = 1.0 / g;
	double a2 = 1.0 / (g * g);

	unsigned int xOffest = (unsigned int)(a1 * frame_width);
	unsigned int yOffset = (unsigned int)(a2 * frame_height);

	//x[n] = (0.5 + a1 * n) % 1;
	//y[n] = (0.5 + a2 * n) % 1;
	//thats happening right here next!!
	unsigned int oldFrameDitherXOffset = oldFrameDitherX + xOffest;
	unsigned int oldFrameDitherYOffset = oldFrameDitherY + yOffset;
	// now here the modulo 1!
	new_positions[0] = oldFrameDitherXOffset % frame_width;
	new_positions[1] = oldFrameDitherYOffset % frame_height;

	return new_positions;

}

void helpers::fromPermuteToFile(const char* filename, Image image) {

	FIBITMAP* bm = FreeImage_Allocate(dither_width, dither_height, 32); 
	fromPermuteToBitmap(image, bm, dither_width, dither_height);
	bool saved = saveImageToFile(filename, bm);
	if (!saved) std::cout << "Uneable to save Permutation File!";
}

void helpers::fromImageToFile(const char* filename, Image image) {

	FIBITMAP* bm = FreeImage_Allocate(dither_width, dither_height, 32);
	fromImageToBitmap(image, bm, dither_width, dither_height);
	bool saved = saveImageToFile(filename, bm);
	if (!saved) std::cout << "Uneable to save Image File!";
}

int helpers::getDitherWith() {

	return this->dither_width;

}

int helpers::getDitherHeight() {

	return this->dither_height;

}