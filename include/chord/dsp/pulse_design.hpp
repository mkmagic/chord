#pragma once

#include <chord/error/status.h>

#include <kfr/all.hpp>

namespace chord::dsp {

enum class PulseType { Gaussian, RRC, RC };

/**
 * @brief Generates coefficients for common communication pulse shapes.
 *
 * Generates the FIR taps for Gaussian, Raised Cosine (RC), or Root-Raised Cosine (RRC)
 * pulse shapes used in communication systems for matched filtering or signal shaping.
 *
 * @param type The type of pulse (PulseType::Gaussian, PulseType::RRC, or PulseType::RC).
 * @param span Number of symbols the pulse spans. Total filter length will be (span * sps + 1).
 * @param sps Samples per symbol (must be > 0).
 * @param beta Rolloff factor for RRC/RC, or BT (bandwidth-time) for Gaussian (must be > 0).
 * @param out Write view where the filter taps will be stored.
 * @return Status code indicating success or the failure reason.
 */
Status design_pulse_shape(PulseType type,
                          size_t span,
                          size_t sps,
                          float beta,
                          kfr::univector_ref<float> out);

}  // namespace chord::dsp
