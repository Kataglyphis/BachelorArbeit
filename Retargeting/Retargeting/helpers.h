#pragma once

#include "d3d11.h"
#include <stdio.h>
#include <iostream>
#include "FreeImage/FreeImage.h"
#include "FreeImage/FreeImagePlus.h"
#include <vector>

using namespace std;
//for moredimensional arrays!!
typedef vector<int> Values;
typedef vector<Values> Column;
typedef vector<Column> Image;

class helpers {
	public:

		helpers();

		const char* blueNoiseFile = "pictures/BlueNoiseCode/FreeBlueNoiseTextures/Data/64_64/HDR_L_0.png";
		FIBITMAP* blueNoiseBitMap;
		int dither_width;
		int dither_height;

		bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** srv, ID3D11Device* g_pd3dDevice, int* width, int* height);
		bool freeImageFunction();
		bool generateSeedPNG();
		bool loadImageFromFile();
		bool deepCopyImage(Image& source, Image& dest, const int image_width, const int image_height);
		bool fromPermuteToBitmap(Image img_data, FIBITMAP* bitmap, const uint32_t image_width, const uint32_t image_height);
		bool fromImageToBitmap(Image& image_data, FIBITMAP* bitmap, const uint32_t image_width, const uint32_t image_height);
		bool getNextDither(Image dither_data, Image& next_dither_data, const uint32_t frame_width, const uint32_t frame_height);
		bool saveImageToFile(const char* filenameToSave, FIBITMAP* retargetBitMap);
		bool loadPNGinArray(const char* fileName, Image& img_data);
		std::vector<int> toroidallyShift(const unsigned int oldFrameDitherX, const unsigned int oldFrameDitherY, const uint32_t frame_width, const uint32_t frame_height);
		void fromPermuteToFile(const char* filename, Image image);
		void fromImageToFile(const char* filename, Image image);
};

