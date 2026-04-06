namespace chord::dsp {

template <typename E>
auto freq_shift(const E&& signal, float fs, float fc){
    auto osc = kfr::phasor(fs / fc);
    return signal * osc;
}

}  // namespace chord::dsp
