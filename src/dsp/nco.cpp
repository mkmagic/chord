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

    const float phase_step = kfr::c_pi<float, 2> * frequency / sample_rate;

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

    const float phase_step = kfr::c_pi<float, 2> * frequency / sample_rate;

    // kfr::counter returns a lazy expression (no allocation). kfr::cos and kfr::sin
    // also lift it into lazy expressions. kfr::make_complex then fuses all three into
    // one composite expression that KFR evaluates in a single SIMD pass upon assignment.
    // Zero heap allocations.
    auto phase_ramp = kfr::counter<float>(state.phase, phase_step);
    out_iq = kfr::make_complex(kfr::cos(phase_ramp), kfr::sin(phase_ramp));

    // Advance state and keep it bounded to avoid precision loss
    state.phase = math::wrap_phase(state.phase + phase_step * static_cast<float>(size));
}

}  // namespace chord::dsp
