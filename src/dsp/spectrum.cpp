#include <chord/dsp/spectrum.hpp>

#include <kfr/dft.hpp>

#include <algorithm>

namespace chord::dsp {

Status estimate_psd(kfr::univector_ref<const kfr::complex<float>> input,
                    size_t n_fft,
                    kfr::window_type window_type,
                    kfr::univector_ref<float> out,
                    SpectrumPsdWorkspace& workspace) {
    size_t proc_size = std::min(input.size(), n_fft);
    if (proc_size == 0) {
        return Status::INPUT_TOO_SMALL;
    }
    if (out.size() < n_fft) {
        return Status::OUTPUT_TOO_SMALL;
    }

    if (workspace.n_fft != n_fft || workspace.temp.size() != n_fft ||
        workspace.temp_buffer.size() != workspace.plan.temp_size ||
        workspace.win.size() != proc_size) {
        return Status::WORKSPACE_MISMATCH;
    }

    std::fill(workspace.temp.begin(), workspace.temp.end(), kfr::complex<float>{0.0f, 0.0f});
    workspace.win = kfr::window(proc_size, window_type, float{});

    // Apply window to input and store in zero-padded temp buffer
    workspace.temp.slice(0, proc_size) = input.slice(0, proc_size) * workspace.win;

    // Perform DFT in-place
    workspace.plan.execute(workspace.temp, workspace.temp, workspace.temp_buffer);

    // PSD = magnitude squared
    out.slice(0, n_fft) = kfr::sqr(kfr::cabs(workspace.temp));

    return Status::OK;
}

Status estimate_psd(kfr::univector_ref<const kfr::complex<float>> input,
                    size_t n_fft,
                    kfr::window_type window_type,
                    kfr::univector_ref<float> out) {
    size_t proc_size = std::min(input.size(), n_fft);
    if (proc_size == 0) {
        return Status::INPUT_TOO_SMALL;
    }
    if (out.size() < n_fft) {
        return Status::OUTPUT_TOO_SMALL;
    }

    SpectrumPsdWorkspace workspace;
    workspace.n_fft = n_fft;
    workspace.plan = kfr::dft_plan<float>(n_fft);
    workspace.temp.resize(n_fft);
    workspace.temp_buffer.resize(workspace.plan.temp_size);
    workspace.win.resize(proc_size);
    return estimate_psd(input, n_fft, window_type, out, workspace);
}

}  // namespace chord::dsp
