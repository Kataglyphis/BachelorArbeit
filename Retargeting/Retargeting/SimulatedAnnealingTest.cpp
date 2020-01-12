#include "SimulatedAnnealingTest.h";

SimulatedAnnealingTest::SimulatedAnnealingTest() : sa(), helper() {

}

void SimulatedAnnealingTest::testPermutation(const char* filename, const unsigned int steps) {

	Image permutation = sa.execute(steps, filename);

	Image next_dither;
	Image original; 

	for (int i = 0; i < helper.dither_width; i++) {

		Column column_org;
		Column column_dither;

		for (int j = 0; j < helper.dither_height; j++) {

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
	helper.getNextDither(original, next_dither, helper.dither_width, helper.dither_height);

	Image appliedPerm = applyPermutationToOriginal(original, permutation);

	const char* steps_str = "steps_";
	int length = snprintf(NULL, 0, "%d", steps);
	char* num_steps = (char*)malloc(length + 1);
	snprintf(num_steps, length + 1, "%d", steps);

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

	helper.fromImageToFile(next_dither_str, next_dither);
	helper.fromImageToFile(permuted_image, appliedPerm);

	//freeing memory
	free(num_steps);
	free(permuted_image);
	free(next_dither_str);

}

Image SimulatedAnnealingTest::applyPermutationToOriginal(Image original, Image permutation) {

	Image permutatedOriginal;
	for (int i = 0; i < helper.dither_width; i++) {

		Column column_org;

		for (int j = 0; j < helper.dither_height; j++) {

			//just assign the standard distribution
			//how this looks like; look at th etop for it 

			using namespace std;
			Values start_values_dither(4, 0);

			column_org.push_back(start_values_dither);

		}

		permutatedOriginal.push_back(column_org);

	}

	helper.deepCopyImage(original, permutatedOriginal, helper.dither_width, helper.dither_height);
	for (int i = 0; i < helper.dither_width; i++) {

		for (int j = 0; j < helper.dither_height; j++) {

			permutatedOriginal[i + permutation[i][j][0]][j + permutation[i][j][1]] = original[i][j];

		}
	}

	return permutatedOriginal;

}