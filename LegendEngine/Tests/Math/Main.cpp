#include <gtest/gtest.h>

#include <LegendEngine/Math/Math.hpp>

using namespace LegendEngine;
using namespace Math;

#define EXPECT_VEC_EQ(a, b) \
    EXPECT_TRUE(a.x == b.x); \
    EXPECT_TRUE(a.y == b.y); \
    EXPECT_TRUE(a.z == b.z)

TEST(Math, VectorEquality)
{
    const Vector3f a(1.0f, 2.0f, 3.0f);
    const Vector3f b(1.0f, 2.0f, 3.0f);

    EXPECT_VEC_EQ(a, b);
}