#pragma once

#include <chord/error/status.h>

#include <kfr/all.hpp>

namespace chord::dsp {

/**
 * @brief Shifts the frequency of a complex signal.
 * 
 * This function shifts the frequency of a complex signal by multiplying it with a complex exponential.
 * 
 * @param signal The input complex signal.
 * @param fs The sample rate of the signal.
 * @param fc The frequency to shift the signal by.
 * @return The frequency-shifted signal.
 */
template <typename E>
auto freq_shift(const E&& signal, float fs, float fc);

}  // namespace chord::dsp

#include "freq_shift.tpp"
