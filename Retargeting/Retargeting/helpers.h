#pragma once

#include "d3d11.h"
#include <stdio.h>
//#include "lpng/png.h"


class helpers {
	public:
		helpers() {}
		bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** srv, ID3D11Device* g_pd3dDevice, int* width, int* height);
};
	