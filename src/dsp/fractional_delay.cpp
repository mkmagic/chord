#include <chord/dsp/fractional_delay.hpp>
#include <bit>
#include <cmath>
#include <kfr/math/interpolation.hpp>

namespace chord::dsp {

FractionalDelayState make_fractional_delay_state(size_t max_delay) {
    FractionalDelayState state;
    // Buffer needs to be large enough for the delay plus the interpolation kernel size.
    // For cubic (4-point), we need safety margin for wrapping and lookahead.
    size_t requested_size = max_delay + 8;
    state.size = std::bit_ceil(requested_size);
    state.mask = state.size - 1;
    state.buffer.resize(state.size, 0.0f);
    state.write_idx = 0;
    return state;
}

namespace {

inline size_t wrap_idx(intptr_t idx, size_t mask) {
    return static_cast<size_t>(idx & static_cast<intptr_t>(mask));
}

} // namespace

void fractional_delay(kfr::univector_ref<const float> in,
                      kfr::univector_ref<float> out,
                      kfr::univector_ref<const float> delays,
                      FractionalDelayState& state,
                      InterpolationType interp) {
    const size_t size = in.size();
    if (size == 0)
        return;

    size_t i = 0;
    constexpr size_t vec_size = 8; // Process in blocks of 8 for SIMD

    const intptr_t mask_val = static_cast<intptr_t>(state.mask);

    for (; i + vec_size <= size; i += vec_size) {
        // 1. Write current block of samples to buffer
        for (size_t j = 0; j < vec_size; ++j) {
            state.buffer[(state.write_idx + j) & state.mask] = in[i + j];
        }

        // 2. Load delays and calculate fractional components
        const kfr::vec<float, vec_size> d = kfr::read<vec_size>(delays.data() + i);
        const kfr::vec<float, vec_size> floor_d = kfr::floor(d);
        const kfr::vec<float, vec_size> frac = d - floor_d;

        // 3. Calculate indices for interpolation points
        // k is the index of the floor delay sample
        const kfr::vec<intptr_t, vec_size> curr_pos_vec = 
            kfr::broadcast<vec_size>(static_cast<intptr_t>(state.write_idx)) + kfr::enumerate<intptr_t, vec_size>();
        const kfr::vec<intptr_t, vec_size> k = curr_pos_vec - kfr::cast<intptr_t>(floor_d);

        if (interp == InterpolationType::Linear) {
            const kfr::vec<float, vec_size> y1 = kfr::gather(state.buffer.data(), kfr::vec<kfr::u32, vec_size>(k & mask_val));
            const kfr::vec<float, vec_size> y2 = kfr::gather(state.buffer.data(), kfr::vec<kfr::u32, vec_size>((k - 1) & mask_val));
            kfr::write(out.data() + i, kfr::linear(frac, y1, y2));
        } else {
            const kfr::vec<float, vec_size> x0 = kfr::gather(state.buffer.data(), kfr::vec<kfr::u32, vec_size>((k + 1) & mask_val));
            const kfr::vec<float, vec_size> x1 = kfr::gather(state.buffer.data(), kfr::vec<kfr::u32, vec_size>(k & mask_val));
            const kfr::vec<float, vec_size> x2 = kfr::gather(state.buffer.data(), kfr::vec<kfr::u32, vec_size>((k - 1) & mask_val));
            const kfr::vec<float, vec_size> x3 = kfr::gather(state.buffer.data(), kfr::vec<kfr::u32, vec_size>((k - 2) & mask_val));

            const kfr::vec<float, vec_size> a0 = x1;
            const kfr::vec<float, vec_size> a1 = 0.5f * (x2 - x0);
            const kfr::vec<float, vec_size> a2 = x0 - 2.5f * x1 + 2.0f * x2 - 0.5f * x3;
            const kfr::vec<float, vec_size> a3 = 0.5f * (x3 - x0) - 1.5f * (x2 - x1);

            kfr::write(out.data() + i, kfr::horner(frac, a0, a1, a2, a3));
        }

        // 4. Advance write pointer
        state.write_idx = (state.write_idx + vec_size) & state.mask;
    }

    // Handle remainder samples scalar-ly
    for (; i < size; ++i) {
        state.buffer[state.write_idx] = in[i];

        const float d = delays[i];
        const float floor_d = std::floor(d);
        const float frac = d - floor_d;

        const intptr_t k = static_cast<intptr_t>(state.write_idx) - static_cast<intptr_t>(floor_d);

        if (interp == InterpolationType::Linear) {
            const float y1 = state.buffer[wrap_idx(k, state.mask)];
            const float y2 = state.buffer[wrap_idx(k - 1, state.mask)];
            out[i] = kfr::linear(frac, y1, y2);
        } else {
            const float x0 = state.buffer[wrap_idx(k + 1, state.mask)];
            const float x1 = state.buffer[wrap_idx(k, state.mask)];
            const float x2 = state.buffer[wrap_idx(k - 1, state.mask)];
            const float x3 = state.buffer[wrap_idx(k - 2, state.mask)];

            const float a0 = x1;
            const float a1 = 0.5f * (x2 - x0);
            const float a2 = x0 - 2.5f * x1 + 2.0f * x2 - 0.5f * x3;
            const float a3 = 0.5f * (x3 - x0) - 1.5f * (x2 - x1);

            out[i] = kfr::horner(frac, a0, a1, a2, a3);
        }

        state.write_idx = (state.write_idx + 1) & state.mask;
    }
}

} // namespace chord::dsp
