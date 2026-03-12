#pragma once

#include <kfr/all.hpp>

namespace chord::math {

/**
 * @brief State container for exponential moving average (EMA).
 */
struct ExponentialMovingAverageState {
    float average{0.0f};
    bool initialized{false};
};

/**
 * @brief Computes an exponential moving average (EMA) on a continuous stream.
 *
 * An Exponential Moving Average acts like a very fast smoothing filter. It tracks the
 * "center of gravity" of a noisy signal in real-time. It requires almost no memory compared
 * to standard moving averages. It is heavily used in Automatic Gain Control (AGC) and signal
 * trackers.
 *
 * @param in Read-only view of the input signal.
 * @param out Write view where the smoothed EMA values will be stored.
 * @param state State maintained across buffer calls to ensure smoothing continuity.
 * @param alpha The smoothing factor [0.0, 1.0]. Smaller alpha = slower response / more smoothing.
 */
void exponential_moving_average(kfr::univector_ref<const float> in,
                                kfr::univector_ref<float> out,
                                ExponentialMovingAverageState& state,
                                float alpha);

/**
 * @brief State container for exponential moving variance (EMV).
 */
struct ExponentialMovingVarianceState {
    float average{0.0f};
    float variance{0.0f};
    bool initialized{false};
};

/**
 * @brief Computes the exponential moving variance (EMV) of a continuous stream.
 *
 * This function continuously estimates the "spread" or power of a signal as it streams in.
 * It tracks both the running average and the running variance simultaneously. It's incredibly
 * useful for squelch algorithms, noise floor estimation, and detecting signal presence.
 *
 * @param in Read-only view of the input signal.
 * @param out_var Write view where the computed variance values will be stored.
 * @param state State maintained across buffer calls to ensure statistical continuity.
 * @param alpha The smoothing factor [0.0, 1.0]. Smaller alpha = slower response / more smoothing.
 */
void exponential_moving_variance(kfr::univector_ref<const float> in,
                                 kfr::univector_ref<float> out_var,
                                 ExponentialMovingVarianceState& state,
                                 float alpha);

}  // namespace chord::math
