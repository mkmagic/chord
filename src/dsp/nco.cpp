#include <chord/dsp/nco.hpp>
#include <chord/math/phase.hpp>

namespace chord::dsp {

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
    state.phase = math::wrap_phase(state.phase + phase_step * static_cast<float>(size));
}

void nco_generate_complex(kfr::univector_ref<kfr::complex<float>> out_iq,
                          NcoState& state,
                          float frequency,
                          float sample_rate) {
    const size_t size = out_iq.size();
    if (size == 0)
        return;

    const float phase_step = 2.0f * kfr::c_pi<float, 1> * frequency / sample_rate;

    // Generate a phase ramp into a temporary buffer using kfr::counter.
    kfr::univector<float> phases(size);
    phases = kfr::counter<float>(state.phase, phase_step);

    // Apply Euler's formula: e^(j*phase) = cos(phase) + j*sin(phase).
    // kfr::make_complex combines two real SIMD vectors into a complex one in a single pass.
    out_iq = kfr::make_complex(kfr::cos(phases), kfr::sin(phases));

    // Advance state and keep it bounded to avoid precision loss
    state.phase = math::wrap_phase(state.phase + phase_step * static_cast<float>(size));
}

}  // namespace chord::dsp
