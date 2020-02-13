#include "Lineary.h"

Lineary::Lineary() : T_0(511), mu(1), current_temperature(T_0), quasiequilibrium(1){

}

Lineary::Lineary(double T_0, int number_of_steps) :  quasiequilibrium(1) {
	this->T_0 = T_0 + 1;
	this->mu = (T_0 / ((double)number_of_steps));
	this->current_temperature = T_0;
}

Lineary::Lineary(double T_0, int number_of_steps, double quasiequilibrium) {

	this->T_0 = T_0;
	this->current_temperature = T_0 + 1;
	this->mu = (T_0 / ((double) number_of_steps));
	this->quasiequilibrium = quasiequilibrium;

}


double Lineary::getTemperature(int step) {

	if (std::fmod(step + 1, quasiequilibrium) == 0) (this->current_temperature = this->T_0 - this->mu * step);

	return this->current_temperature;
}

std::string Lineary::getName() {
	return "LinearyCooldownSchedule";
}

std::string Lineary::getFunction() {

	std::stringstream function;
	function << "f(t) = " << T_0 << "-" << mu << "*t";

	return function.str();
}

float Lineary::getQuasiEq() {
	return 0;
}