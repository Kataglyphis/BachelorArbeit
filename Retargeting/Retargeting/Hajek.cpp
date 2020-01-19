#include "Hajek.h"


Hajek::Hajek():T_0(511) {

}

double Hajek::getTemperature(int step)  {

	double temperature = this->T_0 / std::log(step + 1);

	return temperature;
}

std::string Hajek::getName() {
	return "_HajekCooldownSchedule";
}