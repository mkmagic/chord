#include <chord/dsp/nco.hpp>

namespace chord::dsp {

namespace {
// Helper: wraps phase into [-pi, pi)
inline float wrap_phase(float p) {
    constexpr float TWO_PI = 2.0f * kfr::c_pi<float, 1>;
    p = std::fmod(p + kfr::c_pi<float, 1>, TWO_PI);
    if (p < 0.0f)
        p += TWO_PI;
    return p - kfr::c_pi<float, 1>;
}
}  // namespace

void nco_generate(kfr::univector_ref<float> out_phase,
                  NcoState& state,
                  float frequency,
                  float sample_rate) {
    const size_t size = out_phase.size();
    if (size == 0)
        return;

    const float phase_step = 2.0f * kfr::c_pi<float, 1> * frequency / sample_rate;

    // kfr::counter generates 0, step, 2*step, 3*step, ... as a lazy expression.
    // Adding the scalar state.phase offsets the whole ramp in one vectorized pass.
    out_phase = kfr::counter<float>(state.phase, phase_step);

    // Advance state and keep it bounded to avoid precision loss
    state.phase = wrap_phase(state.phase + phase_step * static_cast<float>(size));
}

void nco_generate_complex(kfr::univector_ref<kfr::complex<float>> out_iq,
                          NcoState& state,
                          float frequency,
                          float sample_rate) {
    const size_t size = out_iq.size();
    if (size == 0)
        return;

    const float phase_step = 2.0f * kfr::c_pi<float, 1> * frequency / sample_rate;

    // Generate the phase ramp first into a temporary, then map e^(j*phase) via kfr::polar.
    // Using kfr::polar(1.0f, phase) = cos(phase) + j*sin(phase), which KFR can vectorize.
    kfr::univector<float> phases(size);
    phases = kfr::counter<float>(state.phase, phase_step);

    // kfr::make_complex(kfr::cos(phases), kfr::sin(phases)) produces the IQ directly.
    out_iq = kfr::make_complex(kfr::cos(phases), kfr::sin(phases));

    // Advance state and keep it bounded to avoid precision loss
    state.phase = wrap_phase(state.phase + phase_step * static_cast<float>(size));
}

}  // namespace chord::dsp
