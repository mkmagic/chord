#include <chord/math/zero_cross.hpp>

namespace chord::math {

void detect_zero_crossings(kfr::univector_ref<const float> in,
                           kfr::univector_ref<uint8_t> out_mask,
                           ZeroCrossState& state) {
    const size_t size = in.size();
    if (size == 0)
        return;

    for (size_t i = 0; i < size; ++i) {
        float current = in[i];
        bool current_positive = (current >= 0.0f);

        if (!state.initialized) {
            out_mask[i] = 0;
            state.initialized = true;
        } else {
            bool previous_positive = (state.previous_sample >= 0.0f);
            out_mask[i] = (current_positive != previous_positive) ? 1 : 0;
        }

        state.previous_sample = current;
    }
}

}  // namespace chord::math
