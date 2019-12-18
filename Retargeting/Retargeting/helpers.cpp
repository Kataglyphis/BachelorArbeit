
#include "helpers.h"
#include "WangHash.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <stdint.h>


bool helpers::LoadTextureFromFile(const char* filename,ID3D11ShaderResourceView** srv , ID3D11Device* g_pd3dDevice, int* width, int* height) {
	
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL,4);
	if (image_data == NULL) return false;

	//texture creation
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
		
	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, srv);
	pTexture->Release();

	*width = image_width;
	*height = image_height;
	stbi_image_free(image_data);

	return true;
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
			color.rgbGreen = (double)i / 64 * 255.0;
			color.rgbBlue = (double)j / 64 * 255.0;
			FreeImage_SetPixelColor(bitmap, i, j, &color);
		}
	}
	FreeImage_Save(FIF_PNG, bitmap, "test.png", 0);

	FreeImage_DeInitialise();

	return true;
}

bool helpers::generateSeedPNG() {
	using namespace std;

	FreeImage_Initialise();

	WangHash hashHelper;

	int width = 1920;
	int height = 720;
	int resolution = 32;
	FIBITMAP* bitmap = FreeImage_Allocate(width, height, resolution, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
	if (!bitmap) exit(1);
	RGBQUAD color;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			uint32_t hash = hashHelper.generate(uint32_t(i + j * width));
			//cout << hash;
			color.rgbRed =(UINT) ((hash & 0xFF000000) >> 24);
			//cout << (UINT)color.rgbRed << "\n";
			color.rgbGreen = (UINT)((hash & 0x00FF0000) >> 16);
			//cout << (UINT)color.rgbGreen << "\n";
			color.rgbBlue = (UINT)(hash & 0x0000FF00) >> 8;
			//cout << (UINT)color.rgbBlue << "\n";
			color.rgbReserved = (UINT)(hash & 0x000000FF);
			//cout << (UINT)color.rgbReserved << "\n";
			FreeImage_SetPixelColor(bitmap, i, j, &color);
		}
	}

	FreeImage_Save(FIF_PNG, bitmap, "seeds_INT.png", PNG_Z_NO_COMPRESSION);

	FreeImage_DeInitialise();

	return true;
}

bool helpers::loadImageFromFile() {
	blueNoiseBitMap = FreeImage_Load(FIF_PNG ,blueNoiseFile,0);

	return true;
}
