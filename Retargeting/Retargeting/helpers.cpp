#include "helpers.h"
#include "WangHash.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <stdint.h>

using namespace std;

helpers::helpers() : blueNoiseBitMap(), dither_width(64), dither_height(64) {

}

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
			color.rgbGreen = (BYTE)(i / 64 * 255.0);
			color.rgbBlue = (BYTE)(j / 64 * 255.0);
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

bool helpers::fromPermuteToBitmap(Image& image_data, FIBITMAP* bitmap, const uint32_t image_width, const uint32_t image_height) {

	RGBQUAD color;

	for (unsigned int i = 0; i < image_width; i++) {

		for (unsigned int j = 0; j < image_height; j++) {

			color.rgbRed = image_data[i][j][0] + 6;
			color.rgbGreen = image_data[i][j][1] + 6;
			//to store permutation we will only need to save in rg - channel !!
			color.rgbBlue = 0xFF;
			color.rgbReserved = i;

			FreeImage_SetPixelColor(bitmap, i, j, &color);
		}
	}

	return true;

}

bool helpers::fromImageToBitmap(Image& image_data, FIBITMAP* bitmap, const uint32_t image_width, const uint32_t image_height) {

	RGBQUAD color;

	for (unsigned int i = 0; i < image_width; i++) {

		for (unsigned int j = 0; j < image_height; j++) {

			color.rgbRed = image_data[i][j][0];
			color.rgbGreen = image_data[i][j][1];
			//to store permutation we will only need to save in rg - channel !!
			color.rgbBlue = image_data[i][j][2];
			color.rgbReserved = image_data[i][j][3];

			FreeImage_SetPixelColor(bitmap, i, j, &color);
		}
	}

	return true;

}

bool helpers::getNextDither(Image& dither_data, Image& next_dither_data, const uint32_t frame_width, const uint32_t frame_height) {

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
	int image_height = FreeImage_GetHeight(bitmap);
	int image_width = FreeImage_GetWidth(bitmap);
	//allocate store in appropriate size
	RGBQUAD color;
	for (int i = 0; i < image_width; i++) {

		for (int j = 0; j < image_height; j++) {

			if (!FreeImage_GetPixelColor(bitmap, i, j, &color)) exit(1);

			image_data[i][j][0] = color.rgbRed;
			image_data[i][j][1] = color.rgbGreen;
			image_data[i][j][2] = color.rgbBlue;
			image_data[i][j][3] = color.rgbReserved;

		}

	}

	return true;
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

}

void helpers::fromImageToFile(const char* filename, Image image) {

	FIBITMAP* bm = FreeImage_Allocate(dither_width, dither_height, 32);
	fromImageToBitmap(image, bm, dither_width, dither_height);
	bool saved = saveImageToFile(filename, bm);

}