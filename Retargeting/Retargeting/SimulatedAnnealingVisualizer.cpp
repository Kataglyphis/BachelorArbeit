#include "SimulatedAnnealingVisualizer.h"

namespace plt = matplotlibcpp;

SimulatedAnnealingVisualizer::SimulatedAnnealingVisualizer() {

    this->schedule = new Hajek();

}

SimulatedAnnealingVisualizer::SimulatedAnnealingVisualizer(AnnealingSchedule* schedule) {

    this->schedule = schedule;

}

void SimulatedAnnealingVisualizer::visualizeEnergyOverSteps(Energy energy) {
    
    // Prepare data.
    int size = energy.size();
    std::vector<int> x(size);
    for (int i = 0; i < energy.size(); ++i) {
        x.at(i) = i;
    }

    // Set the size of output image to 1200x780 pixels
    plt::figure_size(1200, 780);
    // Plot line from given x and y data. Color is selected automatically.
    plt::named_plot("Energy(x), x is Element of Steps", x, energy);

    // Set x-axis to interval [0,numberOfSteps]
    plt::xlim(0, size);
    // Add graph title
    std::stringstream ss;
    ss << "Energy over  " << size << " steps";
    plt::title(ss.str());
    // Enable legend.
    plt::legend();
    // Save the image (file format is determined by the extension)
    std::stringstream ss2;
    ss2 << this->folder_energy << "Energy_" << size << "_steps_" << schedule->getName() << ".png";
    plt::save(ss2.str());
}