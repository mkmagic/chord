#include <chord/dsp/dc_remove.hpp>

namespace chord::dsp {

namespace {

static constexpr float DEFAULT_DC_REMOVAL_CUTOFF_HZ = 1.0f;
static constexpr float DEFAULT_DC_REMOVAL_SAMPLE_RATE_HZ = 1.0f;
static constexpr float DC_REMOVAL_BIQUAD_Q = 0.707f;  // Butterworth Q for flat passband

kfr::iir_state<float, 1> make_dc_filter(float cutoff_hz, float sample_rate) {
    const float normalized = cutoff_hz / sample_rate;
    const kfr::biquad_section<float> section = kfr::biquad_highpass(normalized, DC_REMOVAL_BIQUAD_Q);
    return kfr::iir_state<float, 1>(kfr::iir_params<float, 1>(section));
}

}  // namespace

DcRemoveState::DcRemoveState()
    : filter(make_dc_filter(DEFAULT_DC_REMOVAL_CUTOFF_HZ, DEFAULT_DC_REMOVAL_SAMPLE_RATE_HZ)),
      cutoff_hz(0.0f),
      sample_rate(0.0f),
      initialized(false) {
}

DcRemoveState::DcRemoveState(float cutoff_hz, float sample_rate)
    : filter(make_dc_filter(cutoff_hz, sample_rate)),
      cutoff_hz(cutoff_hz),
      sample_rate(sample_rate),
      initialized(true) {
}

Status dc_remove(kfr::univector_ref<const float> in,
                 kfr::univector_ref<float> out,
                 DcRemoveState& state,
                 float cutoff_hz,
                 float sample_rate) {
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
    if (cutoff_hz <= 0.0f || cutoff_hz >= sample_rate * 0.5f) {
        return Status::INVALID_PARAM;
    }

    if (!state.initialized || state.cutoff_hz != cutoff_hz || state.sample_rate != sample_rate) {
        state.filter = make_dc_filter(cutoff_hz, sample_rate);
        state.cutoff_hz = cutoff_hz;
        state.sample_rate = sample_rate;
        state.initialized = true;
    }

    auto in_slice = in.slice(0, size);
    auto out_slice = out.slice(0, size);
    out_slice = kfr::iir(in_slice, std::ref(state.filter));

    return Status::OK;
}

}  // namespace chord::dsp
