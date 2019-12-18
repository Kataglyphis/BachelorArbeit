#include "SimulatedAnnealing.h"

bool SimulatedAnnealing::fromBlueNoiseToRetarget(const char* blue_noise_filename, uint32_t image_width, uint32_t image_height, const char* retarget_filename){
	
	int*** image_data = (int***)malloc(image_width * image_height * 4 * sizeof(uint32_t));
	loadPNGinArray(blue_noise_filename, image_data);
	FIBITMAP* retarget_fibitmap = FreeImage_Allocate(image_width, image_height, 32);

	return SimulatedAnnealing::saveRetargetImageToFile(retarget_filename, retarget_fibitmap);
}

bool SimulatedAnnealing::toroidallyShift(unsigned int oldFrameDitherX, unsigned int oldFrameDitherY, int* next_dither_x, int* next_dither_y, uint32_t frame_width, uint32_t frame_height) {

	using namespace std;
	double g = 1.32471795724474602596;
	double a1 = 1.0 / g;
	double a2 = 1.0 / (g * g);
	
	unsigned int xOffest = a1 * frame_width;
	unsigned int yOffset = a2 * frame_height;

	//x[n] = (0.5 + a1 * n) % 1;
	//y[n] = (0.5 + a2 * n) % 1;
	//thats happening right here next!!
	unsigned int oldFrameDitherXOffset = oldFrameDitherX + xOffest;
	unsigned int oldFrameDitherYOffset = oldFrameDitherY + yOffset;
	// now here the modulo 1!
	(*next_dither_x) = oldFrameDitherXOffset % frame_width;
	(*next_dither_y) = oldFrameDitherYOffset % frame_height;

	return true;
}

bool SimulatedAnnealing::loadPNGinArray(const char* fileName, int*** image_data) {

	FIBITMAP* bitmap = FreeImage_Load(FIF_PNG, fileName, PNG_DEFAULT);
	int image_height = FreeImage_GetHeight(bitmap);
    int image_width = FreeImage_GetWidth(bitmap);
	//allocate store in appropriate size
	image_data = (int***)malloc(image_width * image_height * sizeof(uint32_t));
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

// https://www.arnoldrenderer.com/research/dither_abstract.pdf
int SimulatedAnnealing::calculateEnergy(int*** image_t, int*** image_next, int** permutation, int width, int height, int numChannelUsed) {
	
	float energy = 0;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			float pixel_position_difference = 0;
			float pixel_value_difference = 0;

			if (i != j) {
				pixel_value_difference += calcPixelDifference(image_t[i][j], image_next[i][j], numChannelUsed);
				//pixel_position_difference += std::pow(, 2) + std::pow(, 2) / (std::pow(2.1, 2);
			}
			energy += std::exp(-pixel_position_difference - std::abs(pixel_value_difference));
		}
	}

	return energy;
}

bool SimulatedAnnealing::saveRetargetImageToFile(const char* filenameToSave, FIBITMAP* retargetBitMap) {

	return FreeImage_Save(FIF_PNG, retargetBitMap, filenameToSave);

}

bool SimulatedAnnealing::execute(uint32_t  number_steps, const uint32_t image_width, const uint32_t image_height) {
	
	//generate a state with starting permutation: this means
	//nothing will be switched; just apply where everything will stay
	//we have a 2D-Array with each entry possessing 2 ints for permutation reasins
	int*** dither_data = (int***)malloc(image_width * image_height * 2 * sizeof(uint32_t));
	SimulatedAnnealing::loadPNGinArray("dither_texture.png", dither_data);
	int*** permutation =(int***) malloc(image_width * image_height * 2 * sizeof(uint32_t));

	for (int i = 0; i < image_width; i++) {

		for (int j = 0; j < image_height; j++) {
			//just assign the standard distribution
			permutation[i][j][0] = i;
			permutation[i][j][1] = j;

		}
		 
	}

	for (int i = 0; i < number_steps; i++) {

		int index_x, index_y;
		int*** candidate;
		int* neighbor_index_x;
		int* neighbor_index_y;
		(*neighbor_index_y) = -1;
		(*neighbor_index_x) = -1;
		SimulatedAnnealing::selectRandomNeighbor(candidate, neighbor_index_x, neighbor_index_y, index_x, index_y, image_width, image_height);
		if ((*neighbor_index_x == -1) | (*neighbor_index_y == -1)) return false;

	}

	FIBITMAP* retargetBitmap = FreeImage_Allocate(image_width, image_height, 32);

	SimulatedAnnealing::saveRetargetImageToFile("retargeted_texture", retargetBitmap);
}
/**
take in consideration, that only local swaps in a radius r = 6 is considered!
*/
bool SimulatedAnnealing::selectRandomNeighbor(int*** candiadate, int* neighbor_index_x, int* neighbor_index_y, int index_x, int index_y, int image_width, int image_height) {

	int random_number_x = (rand() % 6) + 1;
	int random_number_y = (rand() % 6) + 1;
	//random number, either 0 or 1
	int sign_choosing_x = rand() % 2;
	int sign_choosing_y = rand() % 2;
	
	if (sign_choosing_x) {
		random_number_x *= -1;
	} 
	if (sign_choosing_y) {
		random_number_y *= -1;
	}

	int new_index_x = index_x + random_number_x;
	int new_index_y = index_y + random_number_y;

	//make wrapping; 
	if (new_index_x < 0) {
		new_index_x = image_width - new_index_x;
	} else if (new_index_x > image_width) {
		new_index_x = new_index_x - image_width;
	}

	if (new_index_y < 0) {
		new_index_y = image_height - new_index_y;
	} else if (new_index_y > image_height) {
		new_index_y = new_index_y - image_height;
	}

}

bool SimulatedAnnealing::fromArrayToBitmap(int*** image_data, FIBITMAP* bitmap, uint32_t image_height, uint32_t image_width) {

	/**FIBITMAP* bitmap = FreeImage_Load(FIF_PNG, fileName, PNG_DEFAULT);
	int image_height = FreeImage_GetHeight(bitmap);
	int image_width = FreeImage_GetWidth(bitmap);
	//allocate store in appropriate size
	image_data = (int***)malloc(image_width * image_height * sizeof(uint32_t));
	RGBQUAD color;
	for (int i = 0; i < image_width; i++) {
		for (int j = 0; j < image_height; j++) {
			if (!FreeImage_GetPixelColor(bitmap, i, j, &color)) exit(1);
			image_data[i][j][0] = color.rgbRed;
			image_data[i][j][1] = color.rgbGreen;
			image_data[i][j][2] = color.rgbBlue;
			image_data[i][j][3] = color.rgbReserved;
		}
	}*/
	bitmap =  FreeImage_Allocate(image_width, image_height, 32);
	RGBQUAD color;
	for (int i = 0; i < image_width; i++) {
		for (int j = 0; j < image_height; j++) {

			color.rgbRed = image_data[i][j][0];
			color.rgbGreen = image_data[i][j][1];
			color.rgbBlue = image_data[i][j][2];
			color.rgbReserved = image_data[i][j][3];

			FreeImage_SetPixelColor(bitmap, i, j, &color);
		}
	}

	return true;
}

bool SimulatedAnnealing::getNextDither(int*** frame, int*** next_frame, uint32_t frame_width, uint32_t frame_height) {

	for (int i = 0; i < frame_width; i++) {
		for (int j = 0; j < frame_height; j++) {
			int* next_dither_x;
			int* next_dither_y;
			(*next_dither_x) = -1;
			(*next_dither_y) = -1;
			SimulatedAnnealing::toroidallyShift(i, j, next_dither_x, next_dither_y, frame_width, frame_height);
			if (((*next_dither_x) == -1)| ((*next_dither_y) = -1)) return false;
			*next_frame[*next_dither_x][*next_dither_y] = *frame[i][j];
		}
	}
	
	return true;
}