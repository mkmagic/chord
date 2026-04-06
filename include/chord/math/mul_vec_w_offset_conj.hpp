#pragma once

#include <chord/error/status.h>

#include <kfr/all.hpp>

namespace chord::math {

/**
 * @brief Computes the dot product between a part of a vector the the conj of another part of the same vector
 * @param input Read-only view of the complex input signal.
 * @param offset the offset of the second part of the vector from the first part
 * @param status Status code indicating success or failure reason.
 * @param trunc_start the number of samples to truncate from the beginning of the vector (the start index of the first part)
 * @param trunc_end the number of samples to truncate from the end of the vector (the end index of the second part)
 * @return The dot product between the two parts of the vector.
 */
template <typename E>
auto mul_vec_w_offset_conj(const E&& input, size_t offset, Status& status, size_t trunc_start = 0, size_t trunc_end = 0);

}  // namespace chord::math

#include "mul_vec_w_offset_conj.tpp"
