#include <chord/dsp/pulse_design.hpp>

#include <kfr/io/python_plot.hpp>
#include <kfr/math.hpp>

#include <iostream>

using namespace chord::dsp;

int main() {
    size_t span = 8;
    size_t sps = 10;
    size_t length = span * sps + 1;

    kfr::univector<float> rrc(length);
    kfr::univector<float> rc(length);
    kfr::univector<float> gauss(length);

    chord::Status rrc_status = design_pulse_shape(PulseType::RRC, span, sps, 0.35f, rrc);
    chord::Status rc_status = design_pulse_shape(PulseType::RC, span, sps, 0.35f, rc);
    chord::Status gauss_status = design_pulse_shape(PulseType::Gaussian, span, sps, 0.35f, gauss);

    if (rrc_status != chord::Status::OK || rc_status != chord::Status::OK ||
        gauss_status != chord::Status::OK) {
        std::cerr << "Pulse design failed (rrc=" << static_cast<int>(rrc_status)
                  << ", rc=" << static_cast<int>(rc_status)
                  << ", gaussian=" << static_cast<int>(gauss_status) << ")\n";
        return 1;
    }

    std::cout << "Saving plots..." << std::endl;
    kfr::plot_save("rrc_pulse", rrc, "title='Root Raised Cosine (beta=0.35)'");
    kfr::plot_save("rc_pulse", rc, "title='Raised Cosine (beta=0.35)'");
    kfr::plot_save("gauss_pulse", gauss, "title='Gaussian (beta=0.35)'");
    std::cout << "Done! Look for the .svg or .py files in the current directory." << std::endl;

    return 0;
}
