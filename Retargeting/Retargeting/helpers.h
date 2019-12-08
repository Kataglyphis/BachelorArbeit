#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "d3d11.h"

	class helpers {
	public:
		helpers() {};
		bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** srv, ID3D11Device* g_pd3dDevice, int* width, int* height);
	};
	