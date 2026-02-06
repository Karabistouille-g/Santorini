#include <bob.hpp>

Bob::Bob() : iaBuilderFirst(nullptr), iaBuilderSecond(nullptr), playerBuilderFirst(nullptr), playerBuilderSecond(nullptr) {
    
    Board* b = Board::getInstance(); 

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {

            Case* c = b->getCase(i, j);
            Builder* tempBuilder = c->getBuilder();

            if (tempBuilder != nullptr) {

                // Builder de l'IA
                if (iaBuilderFirst == nullptr && tempBuilder->getPlayer() == 1) { // FIXME depend de l'id de l'ia
                    iaBuilderFirst = tempBuilder; 
                }
                if (iaBuilderSecond == nullptr && tempBuilder->getPlayer() == 1) {
                    iaBuilderSecond = tempBuilder;
                }

                // Builder du joueur
                if (playerBuilderFirst == nullptr && tempBuilder->getPlayer() == 0) {
                    playerBuilderSecond = tempBuilder;
                }
                if (playerBuilderSecond == nullptr && tempBuilder->getPlayer() == 0) {
                    playerBuilderSecond = tempBuilder;
                }
            }
        }
    }
}

int Bob::score() {
    int score = 0;

    auto myBuilders = {iaBuilderFirst, iaBuilderSecond};
    auto oppBuilders = {playerBuilderFirst, playerBuilderSecond};

    for (auto builder : myBuilders) {
        if (builder) score += builder->getPosition()->getFloor() * 10;
    }
    for (auto builder : oppBuilders) {
        if (builder) score -= builder->getPosition()->getFloor() * 10;
    }

    for (auto builder : myBuilders) {
        if (builder && builder->getPosition()->getFloor() == 3) return +1000;
    }
    for (auto builder : oppBuilders) {
        if (builder && builder->getPosition()->getFloor() == 3) return -1000;
    }
}

int Bob::minimax(int h, bool isMaximizing) {

    if (h == 0) return score();

    int bestScore = isMaximizing ? -10000 : 10000;
    auto activeBuilders = isMaximizing ? std::vector<Builder*>{iaBuilderFirst, iaBuilderSecond} : std::vector<Builder*>{playerBuilderFirst, playerBuilderSecond};

    for (Builder* builder : activeBuilders) {
        if (!builder) continue;

        Case* currentPos = builder->getPosition();
        int x = currentPos->getX();
        int y = currentPos->getY();

        for (int i = -1; i < 1; i++) {
            for (int j = -1; j < 1; j++) {
                if (i == 0 && j == 0) continue;

                if (builder->moveBuilder(x + i, y + j)) {

                    Case* newPos = builder->getPosition();
                    int newX = newPos->getX();
                    int newY = newPos->getY();

                    for (int k = -1; k < 1; k++) {
                        for (int l = -1; l < 1; l++) {
                            if (k == 0 && l == 0) continue;

                            if (builder->createBuild(newX + k, newY + l)) {

                                int currentRes = minimax(h - 1, !isMaximizing);

                                if (isMaximizing) {
                                    bestScore = std::max(bestScore, currentRes);
                                } else {
                                    bestScore = std::min(bestScore, currentRes);
                                }

                                builder->undoBuild();
                            }
                        }
                    }
                    builder->undoMove();
                }
            }
        }
    }
    return bestScore;
}

void Bob::playTurn() {
    int bestScore = -10000;
    MoveInfo bestMove = {-1, -1, -1, -1, -1};
    Builder* bestBuilder = nullptr;

    for (Builder* builder : {iaBuilderFirst, iaBuilderSecond}) {
        if (!builder) continue;

        Case* pos = builder->getPosition();
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int nx = pos->getX() + dx;
                int ny = pos->getY() + dy;

                if (builder->moveBuilder(nx, ny)) {
                    for (int bx = -1; bx <= 1; bx++) {
                        for (int by = -1; by <= 1; by++) {
                            int bnx = nx + bx;
                            int bny = ny + by;

                            if (builder->createBuild(bnx, bny)) {
                                int score = minimax(2, false); // Profondeur 2
                                if (score > bestScore) {
                                    bestScore = score;
                                    bestMove = {nx, ny, bnx, bny, score};
                                    bestBuilder = builder;
                                }
                                builder->undoBuild();
                            }
                        }
                    }
                    builder->undoMove();
                }
            }
        }
    }

    if (bestBuilder) {
        bestBuilder->moveBuilder(bestMove.moveX, bestMove.moveY);
        bestBuilder->createBuild(bestMove.buildX, bestMove.buildY);
    }
}