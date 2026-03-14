#include "chord/dsp/pulse_design.hpp"

#include <gtest/gtest.h>

TEST(PulseDesignTest, RRC) {
    size_t span = 5;
    size_t sps = 4;
    kfr::univector<float> out(span * sps + 1);

    chord::dsp::design_pulse_shape(chord::dsp::PulseType::RRC, span, sps, 0.35f, out);

    // Ensure symmetry
    size_t length = span * sps + 1;
    for (size_t i = 0; i < length / 2; ++i) {
        EXPECT_NEAR(out[i], out[length - 1 - i], 1e-4);
    }
}
