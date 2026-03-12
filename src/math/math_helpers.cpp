#include <chord/math/math_helpers.hpp>

#include <kfr/all.hpp>

namespace chord::math {

std::expected<kfr::univector<float>, std::string>
multiply_vectors(kfr::univector_ref<const float> a, kfr::univector_ref<const float> b) {
    if (a.size() != b.size()) {
        return std::unexpected("Input vectors must have the same size.");
    }

    kfr::univector<float> result(a.size());
    result = a * b;

    return result;
}

}  // namespace chord::math
