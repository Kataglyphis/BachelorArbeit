#pragma once

#include "d3d11.h"
#include <stdio.h>
#include <iostream>
#include "FreeImage/FreeImage.h"
#include "FreeImage/FreeImagePlus.h"


class helpers {
	public:
		const char* blueNoiseFile = "pictures/BlueNoiseCode/FreeBlueNoiseTextures/Data/64_64/HDR_L_0.png";
		FIBITMAP* blueNoiseBitMap;

		helpers(){};
		bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** srv, ID3D11Device* g_pd3dDevice, int* width, int* height);
		bool freeImageFunction();
		bool generateSeedPNG();
		bool loadImageFromFile();
};

