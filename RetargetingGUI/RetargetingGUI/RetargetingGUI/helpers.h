#pragma once

#include <stdio.h>
#include <iostream>
#include "libs/FreeImage/FreeImage.h"
#include "libs/FreeImage/FreeImagePlus.h"
#include <vector>

using namespace std;

/**for moredimensional arrays!!
define an Image with its corresponding columns*/
typedef vector<int> Values;
typedef vector<Values> Column;
typedef vector<Column> Image;

class helpers {
	public:

		helpers();

		bool freeImageFunction();
		bool loadImageFromFile();
		bool saveImageToFile(const char* filenameToSave, FIBITMAP* retargetBitMap);
		bool loadPNGinArray(const char* fileName, Image& img_data);
		bool getNextDither(Image dither_data, Image& next_dither_data, const uint32_t frame_width, const uint32_t frame_height);
		bool deepCopyImage(Image& source, Image& dest, const int image_width, const int image_height);
		bool fromPermuteToBitmap(Image img_data, FIBITMAP* bitmap, const uint32_t image_width, const uint32_t image_height);
		void fromImageToFile(const char* filename, Image image);
		int getDitherWith();
		int getDitherHeight();

private:

		std::vector<int> toroidallyShift(const unsigned int oldFrameDitherX, const unsigned int oldFrameDitherY, const uint32_t frame_width, const uint32_t frame_height);
		bool fromImageToBitmap(Image& image_data, FIBITMAP* bitmap, const uint32_t image_width, const uint32_t image_height);
		void fromPermuteToFile(const char* filename, Image image);

		const char* blueNoiseFile = "pictures/BlueNoiseCode/FreeBlueNoiseTextures/Data/64_64/HDR_L_0.png";
		int dither_width;
		int dither_height;
		FIBITMAP* blueNoiseBitMap;

};

