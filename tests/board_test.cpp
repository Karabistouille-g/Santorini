#include "gtest/gtest.h"
#include "../include/board.hpp"
#include "../include/case.hpp"

#include <iostream>

TEST(BoardTest, SingletonInstance) {
    Board* board1 = Board::getInstance();
    Board* board2 = Board::getInstance();
    EXPECT_EQ(board1, board2);
}

TEST(BoardTest, GetCase) {
    Board* board = Board::getInstance();

    Case* case00 = board->getCase(0, 0);
    Case* case44 = board->getCase(4, 4);

    ASSERT_NE(case00, nullptr);
    EXPECT_EQ(case00->getX(), 0);
    EXPECT_EQ(case00->getY(), 0);

    ASSERT_NE(case44, nullptr);
    EXPECT_EQ(case44->getX(), 4);
    EXPECT_EQ(case44->getY(), 4);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}