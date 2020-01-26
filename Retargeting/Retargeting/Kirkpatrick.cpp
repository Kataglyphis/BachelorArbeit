#include "Kirkpatrick.h"

Kirkpatrick::Kirkpatrick() : T_0(511.f), mu(0.8), currentTemperature(T_0), quasiequilibrium(2048.f){

}

Kirkpatrick::Kirkpatrick(double T_0, double mu): currentTemperature(511.f), quasiequilibrium(4096) {
	this->T_0 = T_0;
	this->mu = mu;
}

double Kirkpatrick::getTemperature(int step) {

	//specialty when updaating this temperature!!!
	// only if we get to a quasi equilibrium, we will change temperature
	// +1 is hotfix!
	if (std::fmod(step + 1, quasiequilibrium) == 0) (this->currentTemperature = (this->currentTemperature * this->mu));
		
	return this->currentTemperature;
}

std::string Kirkpatrick::getName() {
	return "KirkpatrickCooldownSchedule";
}

std::string Kirkpatrick::getFunction() {

	std::stringstream function;
	function << "f(t) = " << "T_t" << "*" << mu;

	return function.str();
}