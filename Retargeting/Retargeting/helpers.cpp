#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <stdint.h>

#include "helpers.h"
#include "WangHash.h"
using namespace std;

//do not use
helpers::helpers() : blueNoiseBitMap(), dither_width(16), dither_height(16) {

}

helpers::helpers(const char* filename, int dither_width, int dither_height) : blueNoiseBitMap() {

	this->dither_width = dither_width;
	this->dither_height = dither_height;

}

//for testing purposes here added!
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

//also only for testing purposes still here
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

/**
For genereating seed png for our path tracer with given strategy! 
*/
BOOL helpers::generate_seed_png(uint64_t seed_texture_width, uint64_t seed_texture_height, uint64_t resolution, RandomnessStrategy* strategy) {
	
	BOOL result = TRUE;
	using namespace std;

	//bool result = true;
	FreeImage_Initialise();

	FIBITMAP* bitmap = FreeImage_Allocate(seed_texture_width, seed_texture_height, resolution, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
	if (!bitmap) exit(1);
	RGBQUAD color;

	for (uint64_t i = 0; i < seed_texture_width; i++) {

		for (uint64_t j = 0; j < seed_texture_height; j++) {

			uint64_t hash = strategy->generate(uint64_t(i + j * seed_texture_width));

			//cout << hash;
			color.rgbRed =(BYTE) ((hash & 0xFF000000) >> 24);
			//cout << (UINT)color.rgbRed << "\n";
			color.rgbGreen = (BYTE)((hash & 0x00FF0000) >> 16);
			//cout << (UINT)color.rgbGreen << "\n";
			color.rgbBlue = (BYTE)((hash & 0x0000FF00) >> 8);
			//cout << (UINT)color.rgbBlue << "\n";
			color.rgbReserved = (BYTE)(hash & 0x000000FF);
			//cout << (UINT)color.rgbReserved << "\n";
			if (!FreeImage_SetPixelColor(bitmap, i, seed_texture_height - 1 - j, &color)) {
				result = FALSE;
			}
		}
	}

	time_t Zeitstempel;
	Zeitstempel = time(0);
	tm* nun;
	nun = localtime(&Zeitstempel);

	stringstream ss;
	ss << seed_texture_home_folder << "seeds_init_" << strategy->getName() << " " << nun->tm_mday << '.' << nun->tm_mon + 1 
		<< '.' << nun->tm_year + 1900 << " - " << nun->tm_hour << '_' << nun->tm_min << ".png";


	if (!FreeImage_Save(FIF_PNG, bitmap, ss.str().c_str(), PNG_Z_NO_COMPRESSION)) {
		result = FALSE;
	}

	FreeImage_DeInitialise();

	return result;
}

bool helpers::loadImageFromFile() {
	blueNoiseBitMap = FreeImage_Load(FIF_PNG ,blueNoiseFile,0);

	return true;
}

bool helpers::deepCopyImage(Image source, Image& dest, const int image_width, const int image_height) {

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
			float max_step_res_ratio = 255.f / 12.f;
			color.rgbRed = (BYTE)(image_data[i][image_height - 1 - j][0] + 6) * max_step_res_ratio;
			color.rgbGreen =(BYTE)(image_data[i][image_height - 1 - j][1] + 6) * max_step_res_ratio;
			//to store permutation we will only need to save in rg - channel !!
			color.rgbBlue = 0x00;
			color.rgbReserved = 0xFF;

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

int helpers::getDitherWidth() {

	return this->dither_width;

}

int helpers::getDitherHeight() {

	return this->dither_height;

}

void helpers::initializeImage(Image& img) {
	
	using namespace std;
	for (int i = 0; i < dither_width; i++) {

		Column column_org;

		for (int j = 0; j < dither_height; j++) {

			//just assign the standard distribution
			//how this looks like; look at th etop for it 
			Values start_values_dither(4, 0);

			column_org.push_back(start_values_dither);

		}

		img.push_back(column_org);

	}
}

void helpers::initializePermutation(Image& perm) {

	using namespace std;
	for (int i = 0; i < dither_width; i++) {

		Column column_perm;

		for (int j = 0; j < dither_height; j++) {

			//just assign the standard distribution
			//how this looks like; look at th etop for it 
			Values start_values_perm(2, 0);
			column_perm.push_back(start_values_perm);

		}

		perm.push_back(column_perm);

	}

}

void helpers::initializePositions(Image& pos) {

	using namespace std;
	for (int i = 0; i < dither_width; i++) {

		Column column_pos;

		for (int j = 0; j < dither_height; j++) {

			//just assign the standard distribution
			//how this looks like; look at th etop for it 
			Values start_values_pos(2, 0);
			start_values_pos[0] = i;
			start_values_pos[1] = j;
			column_pos.push_back(start_values_pos);

		}

		pos.push_back(column_pos);
	}

}

Image helpers::applyPermutationToOriginal(Image original, Image permutation) {

	Image permutatedOriginal;

	initializeImage(permutatedOriginal);
	deepCopyImage(original, permutatedOriginal, dither_width, dither_height);

	for (int i = 0; i < dither_width; i++) {

		for (int j = 0; j < dither_height; j++) {

			int permutation_coordinates_x = (i + permutation[i][j][0] + dither_width) % dither_width;
			int permutation_coordinates_y = (j + permutation[i][j][1] + dither_height) % dither_height;

			permutatedOriginal[permutation_coordinates_x][permutation_coordinates_y] = original[i][j];
		}
	}

	return permutatedOriginal;

}