#include "Hajek.h"


Hajek::Hajek():T_0(511) {

}

Hajek::Hajek(double T_0) {
	this->T_0 = T_0;
}

double Hajek::getTemperature(int step)  {

	double temperature = this->T_0 / std::log(step + 1);

	return temperature;
}

std::string Hajek::getName() {
	return "HajekCooldownSchedule";
}

std::string Hajek::getFunction() {

	std::stringstream function;
	function << "f(t) = " << T_0 <<"/log(1+t)";
	
	return function.str();
}