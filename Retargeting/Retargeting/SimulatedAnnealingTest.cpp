#include "SimulatedAnnealingTest.h";


SimulatedAnnealingTest::SimulatedAnnealingTest() : helper(), number_steps(100), sa() {
	
	this->schedule = new Hajek();
	Energy energy;
	this->sa = SimulatedAnnealing(number_steps, schedule, energy, true);

}

void SimulatedAnnealingTest::testPermutation(const char* filename) {

	int32_t goodswaps = 0;

	Image permutation = sa.execute(filename, goodswaps);

	Image next_dither;
	Image original; 

	for (int i = 0; i < helper.getDitherWith(); i++) {

		Column column_org;
		Column column_dither;

		for (int j = 0; j < helper.getDitherHeight(); j++) {

			//just assign the standard distribution
			//how this looks like; look at th etop for it 

			using namespace std;
			Values start_values_dither(4, 0);

			column_dither.push_back(start_values_dither);
			column_org.push_back(start_values_dither);

		}

		next_dither.push_back(column_dither);
		original.push_back(column_org);

	}

	helper.loadPNGinArray(filename, original);
	helper.getNextDither(original, next_dither, helper.getDitherWith(), helper.getDitherHeight());

	Image appliedPerm = applyPermutationToOriginal(original, permutation);

	/**
	const char* steps_str = "steps_";
	int length = snprintf(NULL, 0, "%d", goodswaps);
	char* num_steps = (char*)malloc(length + 1);
	snprintf(num_steps, length + 1, "%d", goodswaps);

	//save int in a string
	//itoa(steps, num_steps, 10);
	const char* appending = "_permuted_";
	const char* next_dither_str_app = "next_dither_";

	size_t lenOrg = strlen(filename);
	size_t lenApp = strlen(appending);
	size_t lenNext = strlen(next_dither_str_app);
	size_t lenStepStr = strlen(steps_str);
	size_t lenStepNum = strlen(num_steps);

	char* permuted_image = (char*) malloc(lenStepStr + lenStepNum + lenOrg + lenApp + 1);
	char* next_dither_str = (char*)malloc(lenStepStr + lenStepNum + lenOrg + lenNext + 1);
	
	if (permuted_image && next_dither_str) {
	
	//concatenate for str permutated original 
	memcpy(permuted_image, steps_str, lenStepStr);
	memcpy(permuted_image + lenStepStr, num_steps, lenStepNum);
	memcpy(permuted_image + lenStepStr + lenStepNum, appending, lenApp);
	memcpy(permuted_image + lenStepStr + lenStepNum + lenApp, filename, lenOrg);
	permuted_image[lenOrg + lenApp + lenStepNum + lenStepStr] = '\0';

	//concatenate for next dither png
	memcpy(next_dither_str, next_dither_str_app, lenNext);
	memcpy(next_dither_str + lenNext, filename, lenOrg);
	next_dither_str[lenOrg + lenNext] = '\0';

	}
	else {
		cout << "Memory copy went wrong!";
		exit(1);
	}
	*/
	std::stringstream next_dither_ss;
	next_dither_ss << this->folder_permuted_images << "next_dither_" << filename;
	std::stringstream perm_ss;
	perm_ss << this->folder_permuted_images << "steps_" << goodswaps << "_permuted_"  << this->schedule->getName() << "_" << filename;

	helper.fromImageToFile(next_dither_ss.str().c_str(), next_dither);
	helper.fromImageToFile(perm_ss.str().c_str(), appliedPerm);

	//freeing memory
	/**free(num_steps);
	free(permuted_image);
	free(next_dither_str);*/

}

Image SimulatedAnnealingTest::applyPermutationToOriginal(Image original, Image permutation) {

	Image permutatedOriginal;

	for (int i = 0; i < helper.getDitherWith(); i++) {

		Column column_org;

		for (int j = 0; j < helper.getDitherHeight(); j++) {

			//just assign the standard distribution
			//how this looks like; look at th etop for it 

			using namespace std;
			Values start_values_dither = original[i][j];

			column_org.push_back(start_values_dither);

		}

		permutatedOriginal.push_back(column_org);

	}

	//helper.deepCopyImage(original, permutatedOriginal, helper.dither_width, helper.dither_height);

	
	for (int i = 0; i < helper.getDitherWith(); i++) {

		for (int j = 0; j < helper.getDitherHeight(); j++) {

			permutatedOriginal[i + permutation[i][j][0]][j + permutation[i][j][1]] = original[i][j];

		}
	}

	return permutatedOriginal;

}