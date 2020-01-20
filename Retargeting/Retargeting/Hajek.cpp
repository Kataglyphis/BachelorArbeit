#include "Hajek.h"


Hajek::Hajek():T_0(511), c(1) {

}

Hajek::Hajek(double T_0, double c) {
	this->T_0 = T_0;
	this->c = c;
}

double Hajek::getTemperature(int step)  {

	double temperature = this->c / std::log(step + 1);

	return temperature;
}

std::string Hajek::getName() {
	return "HajekCooldownSchedule";
}

std::string Hajek::getFunction() {

	std::stringstream function;
	function << "f(t) = " << c <<"/log(1+t)";
	
	return function.str();
}