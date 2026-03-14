#include "chord/math/correlation.hpp"

namespace chord::math {

kfr::complex<float> sum_conjugate_product(kfr::univector_ref<const kfr::complex<float>> input,
                                          size_t lag) {
    if (input.size() <= lag) {
        return {0.0f, 0.0f};
    }

    auto current = input.slice(lag, input.size() - lag);
    auto delayed = input.slice(0, input.size() - lag);

    return kfr::sum(current * kfr::cconj(delayed));
}

}  // namespace chord::math
