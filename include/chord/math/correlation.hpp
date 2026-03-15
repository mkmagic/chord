#pragma once

#include <chord/error/status.h>

#include <kfr/all.hpp>

namespace chord::math {

/**
 * @brief Computes the sum of conjugate lagged products.
 *
 * Computes $\\sum_{n=lag}^{N-1} x[n]  x[n - lag]^*$.
 * This evaluates a single point of the digital auto-correlation function.
 * Extremely useful for finding the Carrier Frequency Offset (CFO) linearly,
 * by taking the angle of the resulting sum.
 *
 * @param input Read-only view of the complex input signal.
 * @param out_sum Output for the accumulated complex sum.
 * @param lag The time lag (in samples) to compare against.
 * @return Status code indicating success or failure reason.
 */
Status sum_conjugate_product(kfr::univector_ref<const kfr::complex<float>> input,
                             kfr::complex<float>& out_sum,
                             size_t lag = 1);

}  // namespace chord::math
