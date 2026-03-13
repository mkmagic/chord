#include <chord/math/phase.hpp>

namespace chord::math {

void unwrap_phase(kfr::univector_ref<const float> in,
                  kfr::univector_ref<float> out,
                  PhaseUnwrapState& state,
                  float period) {
    const size_t size = in.size();
    if (size == 0)
        return;

    const float half_period = period / 2.0f;

    for (size_t i = 0; i < size; ++i) {
        float current = in[i];
        float delta = current - state.previous_phase;

        if (delta > half_period) {
            state.wrap_offset -= period;
        } else if (delta < -half_period) {
            state.wrap_offset += period;
        }

        out[i] = current + state.wrap_offset;
        state.previous_phase = current;
    }
}

void instantaneous_frequency(kfr::univector_ref<const float> unwrapped_phase,
                             kfr::univector_ref<float> out,
                             float& previous_phase,
                             float sample_rate) {
    const size_t size = unwrapped_phase.size();
    if (size == 0)
        return;

    for (size_t i = 0; i < size; ++i) {
        float current = unwrapped_phase[i];
        out[i] = (current - previous_phase) * sample_rate;
        previous_phase = current;
    }
}

float wrap_phase(float phase) {
    constexpr float TWO_PI = kfr::c_pi<float, 2>;
    phase = std::fmod(phase + kfr::c_pi<float, 1>, TWO_PI);
    if (phase < 0.0f)
        phase += TWO_PI;
    return phase - kfr::c_pi<float, 1>;
}

}  // namespace chord::math
