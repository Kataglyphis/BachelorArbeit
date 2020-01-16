#include "SimulatedAnnealingVisualizer.h"

namespace plt = matplotlibcpp;

SimulatedAnnealingVisualizer::SimulatedAnnealingVisualizer() {

}

void SimulatedAnnealingVisualizer::visualizeEnergyOverSteps(Energy energy) {
    
    // Prepare data.
    //int n = 5000;
    //std::vector<double> x(n), y(n), z(n), w(n, 2);
    int size = energy.size();
    std::vector<int> x(size);
    for (int i = 0; i < energy.size(); ++i) {
        x.at(i) = i;
        //x.at(i) = i * i;
        //y.at(i) = sin(2 * M_PI * i / 360.0);
        //z.at(i) = log(i);
    }

    // Set the size of output image to 1200x780 pixels
    plt::figure_size(1200, 780);
    // Plot line from given x and y data. Color is selected automatically.
    plt::named_plot("Energy(x), x is Element of Steps", x, energy);
    // Plot a red dashed line from given x and y data.
    //plt::plot(x, w, "r--");
    // Plot a line whose name will show up as "log(x)" in the legend.
    //plt::named_plot("Energy(x), x is Element of Steps", x, z);
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
    ss2 << "./Energy_" << size << "_steps.png";
    plt::save(ss2.str());
}