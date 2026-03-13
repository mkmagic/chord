#include <chord/dsp/fractional_delay.hpp>
#include <cmath>

namespace chord::dsp {

FractionalDelayState make_fractional_delay_state(size_t max_delay) {
    FractionalDelayState state;
    // Buffer needs to be large enough for the delay plus the interpolation kernel size.
    // For cubic (4-point), we need a bit of safety margin.
    state.size = max_delay + 8; 
    state.buffer.resize(state.size, 0.0f);
    state.write_idx = 0;
    return state;
}

namespace {

inline size_t wrap_idx(intptr_t idx, size_t size) {
    while (idx < 0) idx += size;
    return static_cast<size_t>(idx % size);
}

inline float interpolate_linear(float frac, float y0, float y1) {
    return y0 + frac * (y1 - y0);
}

inline float interpolate_cubic(float x, float y0, float y1, float y2, float y3) {
    // 4-point Hermite interpolation
    return y1 + 0.5f * x * (y2 - y0 + x * (2.0f * y0 - 5.0f * y1 + 4.0f * y2 - y3 + x * (3.0f * (y1 - y2) + y3 - y0)));
}

} // namespace

void fractional_delay(kfr::univector_ref<const float> in,
                      kfr::univector_ref<float> out,
                      kfr::univector_ref<const float> delays,
                      FractionalDelayState& state,
                      InterpolationType interp) {
    const size_t size = in.size();
    if (size == 0) return;

    for (size_t i = 0; i < size; ++i) {
        // 1. Write current sample to buffer
        state.buffer[state.write_idx] = in[i];

        float delay = delays[i];
        float floor_d = std::floor(delay);
        float frac = delay - floor_d;
        intptr_t k = static_cast<intptr_t>(state.write_idx) - static_cast<intptr_t>(floor_d);

        if (interp == InterpolationType::Linear) {
            float y0 = state.buffer[wrap_idx(k, state.size)];
            float y1 = state.buffer[wrap_idx(k - 1, state.size)];
            out[i] = interpolate_linear(frac, y0, y1);
        } else {
            // Cubic needs 4 points: x[n-(floor-1)], x[n-floor], x[n-(floor+1)], x[n-(floor+2)]
            float y0 = state.buffer[wrap_idx(k + 1, state.size)];
            float y1 = state.buffer[wrap_idx(k, state.size)];
            float y2 = state.buffer[wrap_idx(k - 1, state.size)];
            float y3 = state.buffer[wrap_idx(k - 2, state.size)];
            out[i] = interpolate_cubic(frac, y0, y1, y2, y3);
        }

        // 2. Advance write pointer
        state.write_idx = (state.write_idx + 1) % state.size;
    }
}

} // namespace chord::dsp
