#include "SimulatedAnnealingTest.h";

SimulatedAnnealingTest::SimulatedAnnealingTest() : helper(), number_steps(10000), sa(), image_width(16), image_height(16) {

	this->filename = "pictures/BlueNoiseCode/FreeBlueNoiseTextures/Data/16_16/LDR_RGBA_0.png";
	this->schedule = new Kirkpatrick();
}


SimulatedAnnealingTest::SimulatedAnnealingTest(const char* filename, int image_width, int image_height) : number_steps(10000) {
	
	this->helper = helpers(filename, image_width, image_height);
	//this->schedule = new ExponentialCoolDown();
	double T_0 = 511;
	double quasieq = 20000.f;
	double mu = 0.95;
	this->schedule = new Kirkpatrick(T_0, mu, quasieq);
	Energy energy;
	this->sa = SimulatedAnnealing(number_steps, schedule, energy, true, image_width, image_height, helper, filename);
	this->filename = filename;

}

void SimulatedAnnealingTest::testPermutation() {

	int32_t goodswaps = 0;

	Image permutation = sa.execute(goodswaps);

	Image next_dither;
	Image original; 

	helper.initializeImage(original);
	helper.initializeImage(next_dither);

	helper.loadPNGinArray(filename, original);
	helper.getNextDither(original, next_dither, helper.getDitherWidth(), helper.getDitherHeight());

	Image appliedPerm = helper.applyPermutationToOriginal(original, permutation);

	time_t Zeitstempel;
	tm* nun;
	Zeitstempel = time(0);
	nun = localtime(&Zeitstempel);

	std::stringstream next_dither_ss;
	next_dither_ss << this->folder_permuted_images << "next_dither_"<< nun->tm_mday << '.' << nun->tm_mon + 1 << '.' << nun->tm_year + 1900 << " - " << nun->tm_hour << '_' << nun->tm_min << this->filename;

	std::stringstream perm_ss;
	perm_ss << this->folder_permuted_images << "steps_" << goodswaps << "_permuted_"  << this->schedule->getName() << "_" << nun->tm_mday << '.' << nun->tm_mon + 1 << '.' << nun->tm_year + 1900 << " - " << nun->tm_hour << '_' << nun->tm_min << this->filename;

	std::stringstream org_ss;
	org_ss << this->folder_permuted_images << "org_loaded_and_saved_" << nun->tm_mday << '.' << nun->tm_mon + 1 << '.' << nun->tm_year + 1900 << " - " << nun->tm_hour << '_' << nun->tm_min << this->filename;

	//helper.fromImageToFile(org_ss.str().c_str(), original);
	helper.fromImageToFile(next_dither_ss.str().c_str(), next_dither);
	helper.fromImageToFile(perm_ss.str().c_str(), appliedPerm);

}