#include "helpers.h"
#include "d3d12.h"
class helpers {
	bool LoadTextureFromFromFile(const char* filename, int* width, int* height) {
		int image_width = 0;
		int image_height = 0;
		unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL,4);
		if (image_data = NULL) return false;
		D3D12_RESOURCE_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = image_width;
		desc.Height = image_height;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Flags =

			return true;
	}
};