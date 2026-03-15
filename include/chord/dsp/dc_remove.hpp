#pragma once

#include <chord/error/status.h>

#include <kfr/all.hpp>

namespace chord::dsp {

/**
 * @brief Stateful DC removal filter configuration.
 */
struct DcRemoveState {
    kfr::iir_state<float, 1> filter;
    float cutoff_hz{0.0f};
    float sample_rate{0.0f};
    bool initialized{false};

    DcRemoveState();
    DcRemoveState(float cutoff_hz, float sample_rate);
};

/**
 * @brief Removes DC offset using a 2nd-order high-pass filter.
 *
 * @param in Read-only input samples.
 * @param out Write view for filtered output samples.
 * @param state Filter state preserved across buffers.
 * @param cutoff_hz High-pass cutoff in Hz.
 * @param sample_rate Input sample rate in Hz.
 * @return Status code indicating success or failure reason.
 */
Status dc_remove(kfr::univector_ref<const float> in,
                 kfr::univector_ref<float> out,
                 DcRemoveState& state,
                 float cutoff_hz,
                 float sample_rate);

}  // namespace chord::dsp
