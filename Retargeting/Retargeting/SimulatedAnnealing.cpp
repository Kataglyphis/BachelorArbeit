#include "SimulatedAnnealing.h"

bool SimulatedAnnealing::fromBlueNoiseToRetarget(const char* filename){
	int*** image_data;
	loadPNGinArray(filename, image_data);
	return true;
}

bool SimulatedAnnealing::toroidallyShift(unsigned int oldFrameDitherX, unsigned int oldFrameDitherY) {

	using namespace std;
	uint32_t frameWidth = 1920;
	uint32_t frameHeight = 720; 
	double g = 1.32471795724474602596;
	double a1 = 1.0 / g;
	double a2 = 1.0 / (g * g);
	
	unsigned int xOffest = a1 * frameWidth;
	unsigned int yOffset = a2 * frameHeight;

	//x[n] = (0.5 + a1 * n) % 1;
	//y[n] = (0.5 + a2 * n) % 1;
	//thats happening right here next!!
	unsigned int oldFrameDitherXOffset = oldFrameDitherX + xOffest;
	unsigned int oldFrameDitherYOffset = oldFrameDitherY + yOffset;
	// now here the modulo 1!
	unsigned int nextFrameDitherX = oldFrameDitherXOffset % frameWidth;
	unsigned int nextFrameDitherY = oldFrameDitherYOffset % frameHeight;

	return true;
}

bool SimulatedAnnealing::loadPNGinArray(const char* fileName, int*** image_data) {
	FIBITMAP* bitmap = FreeImage_Load(FIF_PNG, fileName, PNG_DEFAULT);
	int image_height = FreeImage_GetHeight(bitmap);
    int image_width = FreeImage_GetWidth(bitmap);
	RGBQUAD color;
	for (int i = 0; i < image_width; i++) {
		for (int j = 0; j < image_height; j++) {
			if(!FreeImage_GetPixelColor(bitmap, i, j, &color)) exit(1);
			image_data[i][j][0] = color.rgbRed;
			image_data[i][j][1] = color.rgbGreen;
			image_data[i][j][2] = color.rgbBlue;
			image_data[i][j][3] = color.rgbReserved;
		}
	}
}

float SimulatedAnnealing::calcPixelDifference(int pixelA[4], int pixelB[4], int numChannelUsed) {
	
	int sum = 0;
	for (int i = 0; i < numChannelUsed; i++) {
		sum += std::abs(pixelA[i] - pixelB[i]);
	}
	return sum;
}

int SimulatedAnnealing::calculateEnergy(int*** image_t, int*** image_next, int width, int height, int numChannelUsed) {
	
	float pixel_position_difference = 0;
	float pixel_value_difference = 0;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {

			pixel_value_difference += calcPixelDifference(image_t[i][j], image_next[i][j], numChannelUsed);

		}
	}

}