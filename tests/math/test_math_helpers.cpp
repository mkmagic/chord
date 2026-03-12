#include <chord/math/math_helpers.hpp>

#include <kfr/all.hpp>

#include <array>
#include <gtest/gtest.h>
#include <span>
TEST(MathHelpersTest, MultiplyVectors) {
    kfr::univector<float> a = {1.0f, 2.0f, 3.0f, 4.0f};
    kfr::univector<float> b = {2.0f, 3.0f, 4.0f, 5.0f};

    auto result = chord::math::multiply_vectors(a.ref(), b.ref());

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().size(), 4);
    EXPECT_FLOAT_EQ(result.value()[0], 2.0f);
    EXPECT_FLOAT_EQ(result.value()[1], 6.0f);
    EXPECT_FLOAT_EQ(result.value()[2], 12.0f);
    EXPECT_FLOAT_EQ(result.value()[3], 20.0f);
}

TEST(MathHelpersTest, MultiplyVectorsSizeMismatch) {
    kfr::univector<float> a = {1.0f, 2.0f};
    kfr::univector<float> b = {2.0f, 3.0f, 4.0f};

    auto result = chord::math::multiply_vectors(a.ref(), b.ref());

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), "Input vectors must have the same size.");
}

TEST(MathHelpersTest, InteropWithStdSpanAligned) {
    // 1. Allocate strictly aligned memory (simulating an external audio buffer)
    // 64-byte alignment enables optimal AVX512 load/stores
    alignas(64) std::array<float, 4> raw_a = {1.0f, 2.0f, 3.0f, 4.0f};
    alignas(64) std::array<float, 4> raw_b = {2.0f, 3.0f, 4.0f, 5.0f};

    // 2. Wrap memory in modern C++ standard spans
    std::span<const float> span_a(raw_a);
    std::span<const float> span_b(raw_b);

    // 3. Bridge standard spans to KFR's univector_ref explicitly
    // Actually, KFR univector_ref should natively construct itself from contiguous containers like
    // std::span! Let's pass the non-owning spans directly into our chord API.
    auto result = chord::math::multiply_vectors(span_a, span_b);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().size(), 4);
    EXPECT_FLOAT_EQ(result.value()[0], 2.0f);
    EXPECT_FLOAT_EQ(result.value()[1], 6.0f);
    EXPECT_FLOAT_EQ(result.value()[2], 12.0f);
    EXPECT_FLOAT_EQ(result.value()[3], 20.0f);
}

// A bare-minimum custom span class to test KFR's duck typing constructor
template<typename T>
struct my_span {
    T* ptr;
    size_t sz;

    T* data() const {
        return ptr;
    }
    size_t size() const {
        return sz;
    }
};

TEST(MathHelpersTest, InteropWithCustomSpan) {
    alignas(64) std::array<float, 4> raw_a = {1.0f, 2.0f, 3.0f, 4.0f};
    alignas(64) std::array<float, 4> raw_b = {2.0f, 3.0f, 4.0f, 5.0f};

    my_span<const float> custom_a{raw_a.data(), raw_a.size()};
    my_span<const float> custom_b{raw_b.data(), raw_b.size()};

    // Verify that KFR univector_ref can consume ANY class with .data() and .size()
    auto result = chord::math::multiply_vectors(custom_a, custom_b);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value().size(), 4);
    EXPECT_FLOAT_EQ(result.value()[0], 2.0f);
    EXPECT_FLOAT_EQ(result.value()[1], 6.0f);
    EXPECT_FLOAT_EQ(result.value()[2], 12.0f);
    EXPECT_FLOAT_EQ(result.value()[3], 20.0f);
}
