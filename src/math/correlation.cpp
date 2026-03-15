#include <chord/math/correlation.hpp>

namespace chord::math {

Status sum_conjugate_product(kfr::univector_ref<const kfr::complex<float>> input,
                             kfr::complex<float>& out_sum,
                             size_t lag) {
    if (input.size() <= lag) {
        out_sum = {0.0f, 0.0f};
        return Status::INPUT_TOO_SMALL;
    }

    auto current = input.slice(lag, input.size() - lag);
    auto delayed = input.slice(0, input.size() - lag);

    out_sum = kfr::sum(current * kfr::cconj(delayed));
    return Status::OK;
}

}  // namespace chord::math
