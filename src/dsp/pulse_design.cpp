#include "chord/dsp/pulse_design.hpp"

#include <cmath>

namespace chord::dsp {

static void design_rrc(float beta, size_t length, size_t sps, kfr::univector_ref<float> out) {
    const float center = static_cast<float>(length / 2);
    
    // Evaluate the expression for all elements (disregarding singularities initially)
    auto t = (kfr::counter() - center) / static_cast<float>(sps);

    auto normal_val = (kfr::sin(kfr::c_pi<float> * t * (1.0f - beta)) +
                       4.0f * beta * t * kfr::cos(kfr::c_pi<float> * t * (1.0f + beta))) /
                      (kfr::c_pi<float> * t * (1.0f - kfr::sqr(4.0f * beta * t)));

    // Since normal_val evaluates with singularities, it may produce NaN at those points.
    // That's perfectly fine because we overwrite them explicitly below!
    out.slice(0, length) = normal_val;

    // Handle t = 0 singularity explicitly
    size_t center_idx = length / 2;
    if (center_idx < length) {
        out[center_idx] = 1.0f + beta * (4.0f / kfr::c_pi<float> - 1.0f);
    }

    // Handle t = +/- 1 / (4 * beta) singularity (occurs when sps / (4*beta) is perfectly an integer)
    if (beta > 0.0f) {
        float offset = static_cast<float>(sps) / (4.0f * beta);
        // If offset is an integer, we have singularities at center +/- offset
        if (std::abs(std::fmod(offset, 1.0f)) < 1e-5f) {
            size_t idx1 = static_cast<size_t>(center - offset);
            size_t idx2 = static_cast<size_t>(center + offset);
            
            float limit_t_beta = beta / std::sqrt(2.0f) *
                                ((1.0f + 2.0f / kfr::c_pi<float>) * std::sin(kfr::c_pi<float> / (4.0f * beta)) +
                                 (1.0f - 2.0f / kfr::c_pi<float>) * std::cos(kfr::c_pi<float> / (4.0f * beta)));

            if (idx1 < length) out[idx1] = limit_t_beta;
            if (idx2 < length) out[idx2] = limit_t_beta;
        }
    }
}

static void design_rc(float beta, size_t length, size_t sps, kfr::univector_ref<float> out) {
    const float center = static_cast<float>(length / 2);
    auto t = (kfr::counter() - center) / static_cast<float>(sps);

    // sinc inherently handles the t=0 singularity for the sinc portion
    auto normal_val = kfr::sinc(t) * kfr::cos(kfr::c_pi<float> * beta * t) /
                      (1.0f - kfr::sqr(2.0f * beta * t));

    out.slice(0, length) = normal_val;

    // Handle t = 0 specifically in case of division by zero if beta=0? 
    // Actually kfr::sinc handles t=0, returning 1.0. Denominator is 1.0. So t=0 is 1.0 natively.
    size_t center_idx = length / 2;
    if (center_idx < length) {
        out[center_idx] = 1.0f;
    }

    // Handle t = +/- 1 / (2 * beta) singularity
    if (beta > 0.0f) {
        float offset = static_cast<float>(sps) / (2.0f * beta);
        if (std::abs(std::fmod(offset, 1.0f)) < 1e-5f) {
            size_t idx1 = static_cast<size_t>(center - offset);
            size_t idx2 = static_cast<size_t>(center + offset);
            
            float limit_t_beta = (kfr::c_pi<float> / 4.0f) * std::sin(kfr::c_pi<float> / (2.0f * beta)) / (kfr::c_pi<float> / (2.0f * beta));

            if (idx1 < length) out[idx1] = limit_t_beta;
            if (idx2 < length) out[idx2] = limit_t_beta;
        }
    }
}

static void design_gaussian(float beta, size_t length, size_t sps, kfr::univector_ref<float> out) {
    const float center = static_cast<float>(length / 2);
    const float alpha = std::sqrt(std::log(2.0f) / 2.0f) / beta;
    auto t = (kfr::counter() - center) / static_cast<float>(sps);

    out.slice(0, length) = (std::sqrt(kfr::c_pi<float>) / alpha) *
                           kfr::exp(-kfr::sqr(kfr::c_pi<float> * t / alpha));
}

void design_pulse_shape(PulseType type,
                        size_t span,
                        size_t sps,
                        float beta,
                        kfr::univector_ref<float> out) {
    const size_t length = span * sps + 1;
    if (out.size() < length) {
        return;
    }

    switch (type) {
        case PulseType::RRC:
            design_rrc(beta, length, sps, out);
            break;
        case PulseType::RC:
            design_rc(beta, length, sps, out);
            break;
        case PulseType::Gaussian:
            design_gaussian(beta, length, sps, out);
            break;
    }

    // Normalize filter energy
    auto active_out = out.slice(0, length);
    float energy = kfr::sum(kfr::sqr(active_out));
    if (energy > 0.0f) {
        active_out = active_out / std::sqrt(energy);
    }
}

}  // namespace chord::dsp
