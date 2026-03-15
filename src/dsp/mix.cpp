#include <chord/dsp/mix.hpp>

namespace chord::dsp {

Status mix_complex(kfr::univector_ref<const kfr::complex<float>> in,
                   kfr::univector_ref<const kfr::complex<float>> lo,
                   kfr::univector_ref<kfr::complex<float>> out) {
    const size_t size = in.size();
    if (size == 0) {
        return Status::INPUT_TOO_SMALL;
    }
    if (out.size() < size) {
        return Status::OUTPUT_TOO_SMALL;
    }
    if (lo.size() < size) {
        return Status::INPUT_TOO_SMALL;
    }

    auto in_slice = in.slice(0, size);
    auto lo_slice = lo.slice(0, size);
    auto out_slice = out.slice(0, size);
    out_slice = in_slice * lo_slice;

    return Status::OK;
}

Status frequency_translate(kfr::univector_ref<const kfr::complex<float>> in,
                           kfr::univector_ref<kfr::complex<float>> out,
                           NcoState& state,
                           float frequency,
                           float sample_rate,
                           float gain) {
    const size_t size = in.size();
    if (size == 0) {
        return Status::INPUT_TOO_SMALL;
    }
    if (out.size() < size) {
        return Status::OUTPUT_TOO_SMALL;
    }
    if (sample_rate == 0.0f) {
        return Status::DIVIDE_BY_ZERO;
    }
    if (in.data() == out.data()) {
        return Status::INVALID_PARAM;
    }

    auto out_slice = out.slice(0, size);
    Status status = nco_generate_complex(out_slice, state, frequency, sample_rate, gain);
    if (status != Status::OK) {
        return status;
    }

    out_slice = out_slice * in.slice(0, size);

    return Status::OK;
}

}  // namespace chord::dsp
