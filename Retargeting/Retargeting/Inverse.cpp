#include "Inverse.h"

Inverse::Inverse() : T_0(2000), alpha(2){

}

Inverse::Inverse(double T_0, double alpha) {
	this->T_0 = T_0;
	this->alpha = alpha;
}

double Inverse::getTemperature(int step) {

	double temperature = this->T_0 / ( 1. + (alpha * step));

	return temperature;
}

std::string Inverse::getName() {
	return "InverseCooldownSchedule";
}

std::string Inverse::getFunction() {

	std::stringstream function;
	function << "f(t) = " << T_0 << "/(1 + " << alpha << "*" << "t)";

	return function.str();

}