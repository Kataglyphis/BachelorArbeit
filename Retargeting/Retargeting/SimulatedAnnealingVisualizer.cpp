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

void SimulatedAnnealingVisualizer::visualizeAcceptanceProbabilities(Deltas deltas, Probabilities probs) {
    
    // Prepare data.
    std::vector<std::vector<double>> x, y, z;
    for (double i = 0; i < deltas.size(); i += 1) {
        std::vector<double> x_row, y_row, z_row;
        for (double j = 0; j < deltas.size(); j += 1) {
            x_row.push_back(i);
            y_row.push_back(deltas.at(j));
            z_row.push_back(probs.at(j));
        }
        x.push_back(x_row);
        y.push_back(y_row);
        z.push_back(z_row);
    }
    //int size = probs.size();
    //std::vector<int> x(size);
    //std::vector<int> delta_int(size);
    //for (int i = 0; i < probs.size(); ++i) {
     //   x.at(i) = i;
    //}

    plt::plot_surface(x, y, z);
    // Set the size of output image to 1200x780 pixels
    //plt::show();
    //plt::figure_size(1200, 780);
    // Plot line from given x and y data. Color is selected automatically.
    //plt::named_plot("Acceptance probability when delta < 0", x, probs);

    // Set x-axis to interval [0,numberOfSteps]
    //plt::xlim(0, size);
    // Add graph title
    std::stringstream ss;
    ss << "Acceptance Prob[%] over time";
    //plt::title(ss.str());
    // Enable legend.
   // plt::legend();
    // Save the image (file format is determined by the extension)
    std::stringstream ss2;
    ss2 << this->folder_energy << "Acceptance_Probabilities_over_time_" /*<< size*/ << "_steps_" << schedule->getName() << ".png";
    //plt::save(ss2.str());
}

void SimulatedAnnealingVisualizer::visualizeTemperatureOverSteps(Temperatures temperatures) {

    // Prepare data.
    int size = temperatures.size();
    std::vector<int> x(size);
    for (int i = 0; i < temperatures.size(); ++i) {
        x.at(i) = i;
    }

    // Set the size of output image to 1200x780 pixels
    plt::figure_size(1200, 780);
    // Plot line from given x and y data. Color is selected automatically.
    plt::named_plot("Temperature(x), x is Element of Steps", x, temperatures);

    // Set x-axis to interval [0,numberOfSteps]
    plt::xlim(0, size);
    // Add graph title
    std::stringstream ss;
    ss << "Temperature over  " << size << " steps";
    plt::title(ss.str());
    // Enable legend.
    plt::legend();
    // Save the image (file format is determined by the extension)
    std::stringstream ss2;
    ss2 << this->folder_temperature << "Temperature_" << size << "_steps_" << schedule->getName() << ".png";
    plt::save(ss2.str());

}