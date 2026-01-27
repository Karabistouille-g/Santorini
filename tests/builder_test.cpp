#include "gtest/gtest.h"
#include "../include/builder.hpp"
#include "../include/board.hpp"
#include "../include/case.hpp"

TEST(BuilderTest, Initilization) {
    Board* board = Board::getInstance();
    Builder bob(2, 2);

    Case* pos = bob.getPosition();
    ASSERT_NE(pos, nullptr);
    EXPECT_EQ(pos->getX(), 2);
    EXPECT_EQ(pos->getY(), 2);
}

TEST(BuilderTest, MoveBuilderValid) {
    Board* board = Board::getInstance();
    Builder bob(2, 2);

    EXPECT_TRUE(bob.moveBuilder(2, 3));
    
    Case* pos = bob.getPosition();
    ASSERT_NE(pos, nullptr);
    EXPECT_EQ(pos->getX(), 2);
    EXPECT_EQ(pos->getY(), 3);

    board->resetInstance();
}

TEST(BuilderTest, MoveBuidlerOwnPosition) {
    Board* board = Board::getInstance();
    Builder bob(2, 2);

    EXPECT_FALSE(bob.moveBuilder(2, 2));
    
    Case* pos = bob.getPosition();
    ASSERT_NE(pos, nullptr);
    EXPECT_EQ(pos->getX(), 2);
    EXPECT_EQ(pos->getY(), 2);

    board->resetInstance();
}

TEST(BuilderTest, MoveBuilderTooFar) {
    Board* board = Board::getInstance();
    Builder bob(2, 2);

    EXPECT_FALSE(bob.moveBuilder(4, 4));
    
    Case* pos = bob.getPosition();
    ASSERT_NE(pos, nullptr);
    EXPECT_EQ(pos->getX(), 2);
    EXPECT_EQ(pos->getY(), 2);

    board->resetInstance();
}

TEST(BuilderTest, MoveBuilderTooHigh) {
    Board* board = Board::getInstance();
    Builder bob(2, 2);
    
    Case* target = board->getCase(2, 3);
    ASSERT_NE(target, nullptr);
    for (int i = 0; i < 3; i++) {
        target->addFloor();
    }

    EXPECT_FALSE(bob.moveBuilder(2, 3));
    
    Case* pos = bob.getPosition();
    ASSERT_NE(pos, nullptr);
    EXPECT_EQ(pos->getX(), 2);
    EXPECT_EQ(pos->getY(), 2);

    board->resetInstance();
}

TEST(BuilderTest, MoveBuilderTooLowFirstFloor) {
    Board* board = Board::getInstance();
    Builder bob(2, 2);

    EXPECT_TRUE(bob.moveBuilder(2, 3));
    EXPECT_TRUE(bob.createBuild(2, 2));
    EXPECT_TRUE(bob.moveBuilder(2, 2));
    EXPECT_TRUE(bob.createBuild(2, 3));
    EXPECT_TRUE(bob.createBuild(2, 3));
    EXPECT_TRUE(bob.moveBuilder(2, 3));

    EXPECT_TRUE(bob.moveBuilder(3, 3));
    
    Case* pos = bob.getPosition();
    ASSERT_NE(pos, nullptr);
    EXPECT_EQ(pos->getX(), 3);
    EXPECT_EQ(pos->getY(), 3);

    board->resetInstance();
}

TEST(BuilderTest, MoveBuilderTooLowSecondFloor) {
    Board* board = Board::getInstance();
    Builder bob(2, 2);

    EXPECT_TRUE(bob.moveBuilder(2, 3));
    EXPECT_TRUE(bob.createBuild(2, 2));
    EXPECT_TRUE(bob.moveBuilder(2, 2));
    EXPECT_TRUE(bob.createBuild(2, 3));
    EXPECT_TRUE(bob.createBuild(2, 3));
    EXPECT_TRUE(bob.moveBuilder(2, 3));

    EXPECT_TRUE(bob.moveBuilder(3, 3));
    
    Case* pos = bob.getPosition();
    ASSERT_NE(pos, nullptr);
    EXPECT_EQ(pos->getX(), 3);
    EXPECT_EQ(pos->getY(), 3);

    board->resetInstance();
}

TEST(BuilderTest, MoveBuilderTooLowThirdFloor) {
    Board* board = Board::getInstance();
    Builder bob(2, 2);

    EXPECT_TRUE(bob.moveBuilder(2, 3));
    EXPECT_TRUE(bob.createBuild(2, 2));
    EXPECT_TRUE(bob.moveBuilder(2, 2));
    EXPECT_TRUE(bob.createBuild(2, 3));
    EXPECT_TRUE(bob.createBuild(2, 3));
    EXPECT_TRUE(bob.moveBuilder(2, 3));
    EXPECT_TRUE(bob.createBuild(2, 2));
    EXPECT_TRUE(bob.createBuild(2, 2));
    EXPECT_TRUE(bob.moveBuilder(2, 2));

    EXPECT_TRUE(bob.moveBuilder(1, 1));
    
    Case* pos = bob.getPosition();
    ASSERT_NE(pos, nullptr);
    EXPECT_EQ(pos->getX(), 1);
    EXPECT_EQ(pos->getY(), 1);

    board->resetInstance();
}

TEST(BuilderTest, CreateBuildValid) {
    Board* board = Board::getInstance();
    Builder bob(2, 2);

    EXPECT_TRUE(bob.createBuild(2, 3));
    
    Case* target = bob.getPosition();
    ASSERT_NE(target, nullptr);
    EXPECT_EQ(target->getX(), 2);
    EXPECT_EQ(target->getY(), 2);

    board->resetInstance();
}

TEST(BuilderTest, CreateBuildOwnPosition) {
    Board* board = Board::getInstance();
    Builder bob(2, 2);

    EXPECT_FALSE(bob.createBuild(2, 2));
    
    Case* target = bob.getPosition();
    ASSERT_NE(target, nullptr);
    EXPECT_EQ(target->getX(), 2);
    EXPECT_EQ(target->getY(), 2);

    board->resetInstance();
}

TEST(BuilderTest, CreateBuildTooFar) {
    Board* board = Board::getInstance();
    Builder bob(2, 2);

    EXPECT_FALSE(bob.createBuild(4, 4));
    
    Case* target = bob.getPosition();
    ASSERT_NE(target, nullptr);
    EXPECT_EQ(target->getX(), 2);
    EXPECT_EQ(target->getY(), 2);

    board->resetInstance();
}
