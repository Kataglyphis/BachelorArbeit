#include "stdafx.h"

#include "Hajek.h"


Hajek::Hajek():T_0(511), alpha(1) {

}

Hajek::Hajek(double T_0, double alpha) {
	this->T_0 = T_0;
	this->alpha = alpha;
}

double Hajek::getTemperature(int step)  {

	double temperature = this->T_0 / (1 + alpha * std::log(step + 1));

	return temperature;
}

std::string Hajek::getName() {
	return "HajekCooldownSchedule";
}

std::string Hajek::getFunction() {

	std::stringstream function;
	function << "f(t) = " << T_0 <<"/(1+ " << alpha << "*log(1+t))";
	
	return function.str();
}