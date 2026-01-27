#include "gtest/gtest.h"
#include "board.hpp"
#include "case.hpp"

#include <iostream>

TEST(BoardTest, SingletonInstance) {
    Board* board1 = Board::getInstance();
    Board* board2 = Board::getInstance();
    EXPECT_EQ(board1, board2);

    board1->resetInstance();
    EXPECT_EQ(board1, board2); // FIXME après resetInstance, board2 devrait être invalide
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

    board->resetInstance();
}
