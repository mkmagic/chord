#include <chord/dsp/resample.hpp>

namespace chord::dsp {

size_t resample_output_size(size_t input_size,
                            size_t interpolation_factor,
                            size_t decimation_factor,
                            kfr::sample_rate_conversion_quality quality) {
    if (interpolation_factor == 0 || decimation_factor == 0 || input_size == 0) {
        return 0;
    }
    auto converter =
        kfr::sample_rate_converter<float>(quality, interpolation_factor, decimation_factor);
    return static_cast<size_t>(converter.output_size_for_input(static_cast<kfr::i64>(input_size)));
}

size_t decimate_output_size(size_t input_size,
                            size_t decimation_factor,
                            kfr::sample_rate_conversion_quality quality) {
    return resample_output_size(input_size, 1, decimation_factor, quality);
}

size_t upsample_output_size(size_t input_size,
                            size_t interpolation_factor,
                            kfr::sample_rate_conversion_quality quality) {
    return resample_output_size(input_size, interpolation_factor, 1, quality);
}

Status resample(kfr::univector_ref<const float> in,
                kfr::univector_ref<float> out,
                ResamplerState& state,
                size_t interpolation_factor,
                size_t decimation_factor,
                size_t& out_count,
                kfr::sample_rate_conversion_quality quality) {
    const size_t size = in.size();
    if (size == 0) {
        return Status::INPUT_TOO_SMALL;
    }
    if (interpolation_factor == 0 || decimation_factor == 0) {
        return Status::INVALID_PARAM;
    }

    if (!state.initialized || state.interpolation_factor != interpolation_factor ||
        state.decimation_factor != decimation_factor || state.quality != quality) {
        state.converter =
            kfr::sample_rate_converter<float>(quality, interpolation_factor, decimation_factor);
        state.interpolation_factor = interpolation_factor;
        state.decimation_factor = decimation_factor;
        state.quality = quality;
        state.initialized = true;
    }

    const size_t required = static_cast<size_t>(
        state.converter.output_size_for_input(static_cast<kfr::i64>(size)));
    if (out.size() < required) {
        return Status::OUTPUT_TOO_SMALL;
    }

    out_count = required;

    auto out_slice = out.slice(0, required);
    state.converter.process(out_slice, in);

    return Status::OK;
}

Status decimate(kfr::univector_ref<const float> in,
                kfr::univector_ref<float> out,
                ResamplerState& state,
                size_t decimation_factor,
                size_t& out_count,
                kfr::sample_rate_conversion_quality quality) {
    return resample(in, out, state, 1, decimation_factor, out_count, quality);
}

Status upsample(kfr::univector_ref<const float> in,
                kfr::univector_ref<float> out,
                ResamplerState& state,
                size_t interpolation_factor,
                size_t& out_count,
                kfr::sample_rate_conversion_quality quality) {
    return resample(in, out, state, interpolation_factor, 1, out_count, quality);
}

}  // namespace chord::dsp
