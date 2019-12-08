#pragma once

#include "stb/stb_image.h"
#include "d3d11.h"
#include <cmath>
#include "cppOpt/inc/cppOpt.h"


class helpers {
	public:
		helpers() {}
		bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** srv, ID3D11Device* g_pd3dDevice, int* width, int* height);
		bool simulatedAnnealingBlueNoiseTexture();
};
	