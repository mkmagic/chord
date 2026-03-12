#include <chord/math/stats.hpp>

namespace chord::math {

void exponential_moving_average(kfr::univector_ref<const float> in,
                                kfr::univector_ref<float> out,
                                ExponentialMovingAverageState& state,
                                float alpha) {
    const size_t size = in.size();
    if (size == 0)
        return;

    // Fast path: if alpha is 1.0, it's just a copy
    if (alpha >= 1.0f) {
        for (size_t i = 0; i < size; ++i) {
            out[i] = in[i];
        }
        state.average = in[size - 1];
        state.initialized = true;
        return;
    }

    const float one_minus_alpha = 1.0f - alpha;

    for (size_t i = 0; i < size; ++i) {
        float current = in[i];

        if (!state.initialized) {
            state.average = current;
            state.initialized = true;
        } else {
            state.average = alpha * current + one_minus_alpha * state.average;
        }

        out[i] = state.average;
    }
}

void exponential_moving_variance(kfr::univector_ref<const float> in,
                                 kfr::univector_ref<float> out_var,
                                 ExponentialMovingVarianceState& state,
                                 float alpha) {
    const size_t size = in.size();
    if (size == 0)
        return;

    if (alpha >= 1.0f) {
        for (size_t i = 0; i < size; ++i) {
            out_var[i] = 0.0f;
        }
        state.average = in[size - 1];
        state.variance = 0.0f;
        state.initialized = true;
        return;
    }

    const float one_minus_alpha = 1.0f - alpha;

    for (size_t i = 0; i < size; ++i) {
        float current = in[i];

        if (!state.initialized) {
            state.average = current;
            state.variance = 0.0f;
            state.initialized = true;
        } else {
            // Welford's inspired EMA variance update
            float diff = current - state.average;
            state.average = state.average + alpha * diff;
            // Update variance using the new average
            state.variance = one_minus_alpha * (state.variance + alpha * diff * diff);
        }

        out_var[i] = state.variance;
    }
}

}  // namespace chord::math
