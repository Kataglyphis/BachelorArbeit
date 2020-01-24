#include "stdafx.h"

#include "Lineary.h"

Lineary::Lineary() : T_0(511), mu(1){

}

Lineary::Lineary(double T_0, double mu) {
	this->T_0 = T_0;
	this->mu = mu;
}

double Lineary::getTemperature(int step) {

	double temperature = this->T_0 - this->mu*step;

	return temperature;
}

std::string Lineary::getName() {
	return "LinearyCooldownSchedule";
}

std::string Lineary::getFunction() {

	std::stringstream function;
	function << "f(t) = " << T_0 << "-" << mu << "*t";

	return function.str();
}