#include "ExponentialCoolDown.h"

ExponentialCoolDown::ExponentialCoolDown() :T_0(2000) , alpha(0.95){

}

ExponentialCoolDown::ExponentialCoolDown(double T_0, double alpha) {
	this->T_0 = T_0;
	this->alpha = alpha;
}

double ExponentialCoolDown::getTemperature(int step) {

	double temperature = this->T_0 * std::pow(alpha,step);

	return temperature;
}

std::string ExponentialCoolDown::getName() {
	return "ExponentialCooldownSchedule";
}

std::string ExponentialCoolDown::getFunction() {
	
	std::stringstream function;
	function << "f(t) = " << T_0 << "*(" << alpha << "^" << "t)";

	return function.str();

}