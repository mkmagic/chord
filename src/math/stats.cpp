#include <chord/math/stats.hpp>

#include <algorithm>

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
        out = in;
        state.average = in[size - 1];
        state.initialized = true;
        return;
    }

    const float one_minus_alpha = 1.0f - alpha;
    size_t start_index = 0;

    if (!state.initialized) {
        const float current = in[0];
        state.average = current;
        state.initialized = true;
        out[0] = current;
        if (size == 1) {
            return;
        }
        start_index = 1;
    }

    if (start_index < size) {
        const kfr::biquad_section<float> section(1.0f, -one_minus_alpha, 0.0f, alpha, 0.0f, 0.0f);
        kfr::iir_state<float, 1> filter{ kfr::iir_params<float, 1>(section) };
        filter.state.s1[0] = one_minus_alpha * state.average;
        filter.state.s2[0] = 0.0f;
        filter.state.out[0] = state.average;

        auto in_slice = in.slice(start_index, size - start_index);
        auto out_slice = out.slice(start_index, size - start_index);
        out_slice = kfr::iir(in_slice, std::ref(filter));
        state.average = out[size - 1];
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
        std::fill_n(out_var.data(), size, 0.0f);
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
