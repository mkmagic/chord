#include <chord/math/complex.hpp>

namespace chord::math {

void conjugate_multiply(kfr::univector_ref<const kfr::complex<float>> in1,
                        kfr::univector_ref<const kfr::complex<float>> in2,
                        kfr::univector_ref<kfr::complex<float>> out) {
    const size_t size = in1.size();
    if (size == 0)
        return;

    // Using explicit loop to ensure strict univector_ref boundary tracking
    // without invoking KFR expression template hidden allocations
    for (size_t i = 0; i < size; ++i) {
        kfr::complex<float> a = in1[i];
        kfr::complex<float> b = in2[i];
        // Complex conjugate multiply: (a_r + j a_i) * (b_r - j b_i)
        // r = a_r * b_r + a_i * b_i
        // i = a_i * b_r - a_r * b_i

        float r = a.real() * b.real() + a.imag() * b.imag();
        float j = a.imag() * b.real() - a.real() * b.imag();

        out[i] = kfr::complex<float>(r, j);
    }
}

void magnitude_squared(kfr::univector_ref<const kfr::complex<float>> in,
                       kfr::univector_ref<float> out) {
    const size_t size = in.size();
    if (size == 0)
        return;

    for (size_t i = 0; i < size; ++i) {
        float r = in[i].real();
        float j = in[i].imag();
        out[i] = (r * r) + (j * j);
    }
}

}  // namespace chord::math
