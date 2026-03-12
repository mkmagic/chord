#pragma once

#include <kfr/all.hpp>

#include <expected>
#include <string>

namespace chord::math {

/**
 * @brief Multiplies two vectors element-wise using KFR SIMD operations.
 *
 * @param a The first input vector.
 * @param b The second input vector.
 * @return std::expected<kfr::univector<float>, std::string> The resulting vector on success, or an
 * error message string on failure.
 */
std::expected<kfr::univector<float>, std::string>
multiply_vectors(kfr::univector_ref<const float> a, kfr::univector_ref<const float> b);

}  // namespace chord::math
