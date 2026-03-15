#include <chord/math/zero_cross.hpp>

#include <gtest/gtest.h>

using chord::math::detect_zero_crossings;
using chord::math::ZeroCrossState;
using namespace kfr;

TEST(ZeroCrossTest, ConstantZero) {
    univector<float> in(10, 0.0f);
    univector<uint8_t> out(10);

    ZeroCrossState state;
    chord::Status status = detect_zero_crossings(in.ref(), out.ref(), state);

    EXPECT_EQ(status, chord::Status::OK);

    for (size_t i = 0; i < out.size(); ++i) {
        EXPECT_EQ(out[i], 0);
    }
}

TEST(ZeroCrossTest, SingleCrossing) {
    univector<float> in = {-1.0f, -0.5f, 0.5f, 1.0f};
    univector<uint8_t> out(in.size());

    ZeroCrossState state;
    chord::Status status = detect_zero_crossings(in.ref(), out.ref(), state);

    EXPECT_EQ(status, chord::Status::OK);

    EXPECT_EQ(out[0], 0);  // starts negative
    EXPECT_EQ(out[1], 0);
    EXPECT_EQ(out[2], 1);  // crossed from negative to positive
    EXPECT_EQ(out[3], 0);
}

TEST(ZeroCrossTest, StreamingCrossings) {
    univector<float> block1 = {1.0f, 1.0f, 1.0f};
    univector<float> block2 = {-1.0f, -1.0f, -1.0f};

    univector<uint8_t> out1(block1.size());
    univector<uint8_t> out2(block2.size());

    ZeroCrossState state;
    chord::Status status1 = detect_zero_crossings(block1.ref(), out1.ref(), state);
    EXPECT_EQ(status1, chord::Status::OK);
    EXPECT_EQ(out1[0], 0);
    EXPECT_EQ(out1[1], 0);
    EXPECT_EQ(out1[2], 0);

    chord::Status status2 = detect_zero_crossings(block2.ref(), out2.ref(), state);
    EXPECT_EQ(status2, chord::Status::OK);
    EXPECT_EQ(out2[0], 1);  // Exact crossing on block boundary
    EXPECT_EQ(out2[1], 0);
    EXPECT_EQ(out2[2], 0);
}
