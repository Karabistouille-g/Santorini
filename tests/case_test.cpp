#include "gtest/gtest.h"

#include "case.hpp"
#include "builder.hpp"

#include <iostream>

TEST(CaseTest, Initialization) {
    Case c(2, 3);
    EXPECT_EQ(c.getX(), 2);
    EXPECT_EQ(c.getY(), 3);
    EXPECT_EQ(c.getFloor(), 0);
}

TEST(CaseTest, AddFloor) {
    Case c(0, 0);

    EXPECT_EQ(c.getFloor(), 0);

    EXPECT_TRUE(c.addFloor());
    EXPECT_EQ(c.getFloor(), 1);

    EXPECT_TRUE(c.addFloor());
    EXPECT_EQ(c.getFloor(), 2);

    EXPECT_TRUE(c.addFloor());
    EXPECT_EQ(c.getFloor(), 3);

    EXPECT_TRUE(c.addFloor());
    EXPECT_EQ(c.getFloor(), 4);

    EXPECT_FALSE(c.addFloor());
    EXPECT_EQ(c.getFloor(), 4);
}