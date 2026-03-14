#include "chord/dsp/spectrum.hpp"

#include <kfr/dft.hpp>

namespace chord::dsp {

void estimate_psd(kfr::univector_ref<const kfr::complex<float>> input,
                  size_t n_fft,
                  kfr::window_type window_type,
                  kfr::univector_ref<float> out) {
    size_t proc_size = std::min(input.size(), n_fft);
    if (proc_size == 0 || out.size() < n_fft) {
        return;
    }

    kfr::dft_plan<float> plan(n_fft);

    kfr::univector<kfr::complex<float>> temp(n_fft, kfr::complex<float>{0.0f, 0.0f});
    kfr::univector<float> win = kfr::window(proc_size, window_type, float{});

    // Apply window to input and store in zero-padded temp buffer
    temp.slice(0, proc_size) = input.slice(0, proc_size) * win;

    // Perform DFT in-place
    kfr::univector<kfr::u8> temp_buffer(plan.temp_size);
    plan.execute(temp, temp, temp_buffer);

    // PSD = magnitude squared
    out.slice(0, n_fft) = kfr::sqr(kfr::cabs(temp));
}

}  // namespace chord::dsp
