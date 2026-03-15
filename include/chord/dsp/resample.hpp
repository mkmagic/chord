#pragma once

#include <chord/error/status.h>

#include <kfr/dsp/sample_rate_conversion.hpp>

namespace chord::dsp {

/**
 * @brief Stateful resampler configuration.
 */
struct ResamplerState {
    kfr::samplerate_converter<float> converter;
    size_t interpolation_factor{0};
    size_t decimation_factor{0};
    kfr::sample_rate_conversion_quality quality{kfr::sample_rate_conversion_quality::normal};
    bool initialized{false};
};

/**
 * @brief Returns the output size for a fresh resampler instance.
 *
 * Call this to pre-size a destination buffer when you are resampling a single block
 * with a new state (or when you want an upper bound for the first block). For
 * streaming use, prefer the `out_count` returned by `resample` for each chunk.
 */
size_t resample_output_size(size_t input_size,
                            size_t interpolation_factor,
                            size_t decimation_factor,
                            kfr::sample_rate_conversion_quality quality =
                                kfr::sample_rate_conversion_quality::normal);

/**
 * @brief Returns the output size for a fresh decimator instance.
 *
 * Call this when you need to allocate an output buffer for a single decimation
 * call with a new state. For streaming, use the `out_count` returned by `decimate`.
 */
size_t decimate_output_size(size_t input_size,
                            size_t decimation_factor,
                            kfr::sample_rate_conversion_quality quality =
                                kfr::sample_rate_conversion_quality::normal);

/**
 * @brief Returns the output size for a fresh upsampler instance.
 *
 * Call this when you need to allocate an output buffer for a single upsampling
 * call with a new state. For streaming, use the `out_count` returned by `upsample`.
 */
size_t upsample_output_size(size_t input_size,
                            size_t interpolation_factor,
                            kfr::sample_rate_conversion_quality quality =
                                kfr::sample_rate_conversion_quality::normal);

/**
 * @brief Resamples input samples by interpolation/decimation and returns output count.
 *
 * @param in Read-only input samples.
 * @param out Write view for resampled output samples.
 * @param state Converter state preserved across buffers.
 * @param interpolation_factor Integer interpolation factor (> 0).
 * @param decimation_factor Integer decimation factor (> 0).
 * @param out_count Number of output samples written to `out`.
 * @param quality Resampling quality (default normal).
 * @return Status code indicating success or failure reason.
 */
Status resample(kfr::univector_ref<const float> in,
                kfr::univector_ref<float> out,
                ResamplerState& state,
                size_t interpolation_factor,
                size_t decimation_factor,
                size_t& out_count,
                kfr::sample_rate_conversion_quality quality =
                    kfr::sample_rate_conversion_quality::normal);

/**
 * @brief Decimates input samples by an integer factor and returns output count.
 *
 * @param in Read-only input samples.
 * @param out Write view for decimated output samples.
 * @param state Converter state preserved across buffers.
 * @param decimation_factor Integer decimation factor (> 0).
 * @param out_count Number of output samples written to `out`.
 * @param quality Resampling quality (default normal).
 * @return Status code indicating success or failure reason.
 */
Status decimate(kfr::univector_ref<const float> in,
                kfr::univector_ref<float> out,
                ResamplerState& state,
                size_t decimation_factor,
                size_t& out_count,
                kfr::sample_rate_conversion_quality quality =
                    kfr::sample_rate_conversion_quality::normal);

/**
 * @brief Upsamples input samples by an integer factor and returns output count.
 *
 * @param in Read-only input samples.
 * @param out Write view for upsampled output samples.
 * @param state Converter state preserved across buffers.
 * @param interpolation_factor Integer interpolation factor (> 0).
 * @param out_count Number of output samples written to `out`.
 * @param quality Resampling quality (default normal).
 * @return Status code indicating success or failure reason.
 */
Status upsample(kfr::univector_ref<const float> in,
                kfr::univector_ref<float> out,
                ResamplerState& state,
                size_t interpolation_factor,
                size_t& out_count,
                kfr::sample_rate_conversion_quality quality =
                    kfr::sample_rate_conversion_quality::normal);

}  // namespace chord::dsp
