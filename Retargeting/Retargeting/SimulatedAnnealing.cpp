#include "SimulatedAnnealing.h"

bool SimulatedAnnealing::fromBlueNoiseToRetarget(const char* blue_noise_filename, uint32_t image_width, uint32_t image_height, const char* retarget_filename){
	
	Image* image_data;
	loadPNGinArray(blue_noise_filename, image_data);
	FIBITMAP* retarget_fibitmap = FreeImage_Allocate(image_width, image_height, 32);

	return SimulatedAnnealing::saveRetargetImageToFile(retarget_filename, retarget_fibitmap);
}

std::vector<int> SimulatedAnnealing::toroidallyShift(unsigned int oldFrameDitherX, unsigned int oldFrameDitherY, uint32_t frame_width, uint32_t frame_height) {

	std::vector<int> new_positions(2, 0);
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
	new_positions[0] = oldFrameDitherXOffset % frame_width;
	new_positions[1] = oldFrameDitherYOffset % frame_height;

	return new_positions;
}

bool SimulatedAnnealing::loadPNGinArray(const char* fileName, Image* image_data) {

	FIBITMAP* bitmap = FreeImage_Load(FIF_PNG, fileName, PNG_DEFAULT);
	int image_height = FreeImage_GetHeight(bitmap);
    int image_width = FreeImage_GetWidth(bitmap);
	//allocate store in appropriate size
	RGBQUAD color;
	for (int i = 0; i < image_width; i++) {

		Row row(image_width);

		for (int j = 0; j < image_height; j++) {

			if(!FreeImage_GetPixelColor(bitmap, i, j, &color)) exit(1);
			Pixel pixel;
			pixel.push_back(color.rgbRed);
			pixel.push_back(color.rgbGreen);
			pixel.push_back(color.rgbBlue);
			pixel.push_back(color.rgbReserved);
			/**image_data[i][j][0] = color.rgbRed;
			image_data[i][j][1] = color.rgbGreen;
			image_data[i][j][2] = color.rgbBlue;
			image_data[i][j][3] = color.rgbReserved;*/
			row[j] = pixel;
		}
		(*image_data).push_back(row);
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

bool SimulatedAnnealing::execute(uint32_t  number_steps, const char* filename, const uint32_t image_width, const uint32_t image_height) {
	
	//generate a state with starting permutation: this means
	//nothing will be switched; just apply where everything will stay
	//we have a 2D-Array with each entry possessing 2 ints for permutation reasins
	Image* dither_data;
	SimulatedAnnealing::loadPNGinArray("dither_texture.png", dither_data);

	Image permutation_data;
	for (int i = 0; i < image_width; i++) {
		
		Row row(image_width);

		for (int j = 0; j < image_height; j++) {
			//just assign the standard distribution
			
			
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

bool SimulatedAnnealing::fromArrayToBitmap(Image* image_data, FIBITMAP* bitmap, uint32_t image_height, uint32_t image_width) {

	bitmap =  FreeImage_Allocate(image_width, image_height, 32);
	RGBQUAD color;
	for (int i = 0; i < image_width; i++) {
		
		Row row = (*image_data)[i];

		for (int j = 0; j < image_height; j++) {

			color.rgbRed = row[j][0];
			color.rgbGreen = row[j][1];
			color.rgbBlue = row[j][2];
			color.rgbReserved = row[j][3];

			FreeImage_SetPixelColor(bitmap, i, j, &color);
		}
	}

	return true;
}

bool SimulatedAnnealing::getNextDither(Image* dither_data, Image* next_dither_data, uint32_t frame_width, uint32_t frame_height) {

	std::vector<int> new_dither_positions(2,0);

	for (int i = 0; i < frame_width; i++) {
		for (int j = 0; j < frame_height; j++) {
			
			new_dither_positions = SimulatedAnnealing::toroidallyShift(i, j, frame_width, frame_height);
			int new_dither_x = new_dither_positions[0];
			int new_dither_y = new_dither_positions[1];
			(*next_dither_data)[new_dither_x][new_dither_y] = (*dither_data)[i][j];
			//if something went wrong return false
			if (new_dither_positions.size != 2 /**|| ((new_dither_positions[0]==0) && (new_dither_positions[1] == 0))*/) return false;

		}
	}
	
	return true;
}