#include "chord/dsp/pulse_design.hpp"

#include <cmath>

namespace chord::dsp {

void design_pulse_shape(PulseType type,
                        size_t span,
                        size_t sps,
                        float beta,
                        kfr::univector_ref<float> out) {
    const size_t length = span * sps + 1;
    if (out.size() < length) {
        return;
    }

    const int center = static_cast<int>(length) / 2;

    for (int i = 0; i < static_cast<int>(length); ++i) {
        const float t = static_cast<float>(i - center) / static_cast<float>(sps);

        if (type == PulseType::RRC) {
            if (t == 0.0f) {
                out[i] = 1.0f + beta * (4.0f / kfr::c_pi<float> - 1.0f);
            } else if (std::abs(std::abs(t) - 1.0f / (4.0f * beta)) < 1e-5f) {
                out[i] =
                    beta / std::sqrt(2.0f) *
                    ((1.0f + 2.0f / kfr::c_pi<float>) *std::sin(kfr::c_pi<float> / (4.0f * beta)) +
                     (1.0f - 2.0f / kfr::c_pi<float>) *std::cos(kfr::c_pi<float> / (4.0f * beta)));
            } else {
                out[i] = (std::sin(kfr::c_pi<float> * t * (1.0f - beta)) +
                          4.0f * beta * t * std::cos(kfr::c_pi<float> * t * (1.0f + beta))) /
                         (kfr::c_pi<float> * t * (1.0f - std::pow(4.0f * beta * t, 2.0f)));
            }
        } else if (type == PulseType::Gaussian) {
            const float alpha = std::sqrt(std::log(2.0f) / 2.0f) / beta;
            out[i] = (std::sqrt(kfr::c_pi<float>) / alpha) *
                     std::exp(-std::pow(kfr::c_pi<float> * t / alpha, 2.0f));
        } else if (type == PulseType::RC) {
            if (t == 0.0f) {
                out[i] = 1.0f;
            } else if (std::abs(std::abs(t) - 1.0f / (2.0f * beta)) < 1e-5f) {
                out[i] = (kfr::c_pi<float> / 4.0f) * std::sin(kfr::c_pi<float> / (2.0f * beta)) /
                         (kfr::c_pi<float> / (2.0f * beta));
            } else {
                out[i] = (std::sin(kfr::c_pi<float> * t) / (kfr::c_pi<float> * t)) *
                         std::cos(kfr::c_pi<float> * beta * t) /
                         (1.0f - std::pow(2.0f * beta * t, 2.0f));
            }
        }
    }

    // Normalize filter energy
    auto active_out = out.slice(0, length);
    float energy = kfr::sum(kfr::sqr(active_out));
    if (energy > 0.0f) {
        active_out = active_out / std::sqrt(energy);
    }
}

}  // namespace chord::dsp
