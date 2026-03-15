#include <chord/demod/fm.hpp>

namespace chord::demod {

Status fm_demodulate(kfr::univector_ref<const kfr::complex<float>> in,
                     kfr::univector_ref<float> out,
                     FmDemodulatorState& state,
                     float gain) {
    const size_t size = in.size();
    if (size == 0)
        return Status::INPUT_TOO_SMALL;
    if (out.size() < size)
        return Status::OUTPUT_TOO_SMALL;

    // 1. Handle the boundary condition manually to bridge the discontinuous buffer
    // phase difference: arg( current * conj(previous) )
    out[0] = kfr::carg(in[0] * kfr::cconj(state.previous_sample)) * gain;

    // 2. Fast-path: offload the remainder of the buffer directly to KFR's
    // internal expression templates. The compiler will automatically vectorize
    // these contiguous slices together natively into AVX/SSE instructions.
    if (size > 1) {
        auto in_current = in.slice(1);
        auto in_delayed = in.slice(0, size - 1);

        out.slice(1) = kfr::carg(in_current * kfr::cconj(in_delayed)) * gain;
    }

    // 3. Store the precise final sample for the next buffer call
    state.previous_sample = in[size - 1];

    return Status::OK;
}

}  // namespace chord::demod
