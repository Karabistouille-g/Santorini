#include <bob.hpp>

Bob::Bob(int difficulty)
    : iaBuilderFirst(nullptr),
      iaBuilderSecond(nullptr),
      playerBuilderFirst(nullptr),
      playerBuilderSecond(nullptr),
      difficulty_(difficulty)
{
    b = Board::getInstance();

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            Case* c = b->getCase(i, j);
            Builder* tempBuilder = c->getBuilder();
            if (tempBuilder != nullptr) {
                if (tempBuilder->getPlayer() == 1) {
                    if (!iaBuilderFirst) iaBuilderFirst = tempBuilder;
                    else if (!iaBuilderSecond) iaBuilderSecond = tempBuilder;
                } else {
                    if (!playerBuilderFirst) playerBuilderFirst = tempBuilder;
                    else if (!playerBuilderSecond) playerBuilderSecond = tempBuilder;
                }
            }
        }
    }
}

int Bob::score() {
    Builder* iaBuilders[2]     = {iaBuilderFirst, iaBuilderSecond};
    Builder* playerBuilders[2] = {playerBuilderFirst, playerBuilderSecond};

    // Victoire/défaite immédiate
    for (int i = 0; i < 2; i++) {
        if (iaBuilders[i] && iaBuilders[i]->getPosition()->getFloor() == 3)     return 20000;
    }
    for (int i = 0; i < 2; i++) {
        if (playerBuilders[i] && playerBuilders[i]->getPosition()->getFloor() == 3) return -20000;
    }

    int totalScore = 0;

    if (difficulty_ == 1) {
        for (int i = 0; i < 2; i++) {
            if (iaBuilders[i]) totalScore += iaBuilders[i]->getPosition()->getFloor() * 10;
        }
    }
    else if (difficulty_ == 2) {
        for (int i = 0; i < 2; i++) {
            if (!iaBuilders[i]) continue;
            int x = iaBuilders[i]->getPosition()->getX();
            int y = iaBuilders[i]->getPosition()->getY();
            int h = iaBuilders[i]->getPosition()->getFloor();
            totalScore += h * 30;
            if (x >= 1 && x <= 3 && y >= 1 && y <= 3) totalScore += 10;
        }
    }
    else {
        for (int i = 0; i < 2; i++) {
            if (!iaBuilders[i]) continue;
            int x = iaBuilders[i]->getPosition()->getX();
            int y = iaBuilders[i]->getPosition()->getY();
            int h = iaBuilders[i]->getPosition()->getFloor();
            totalScore += h * 100;
            if (x == 2 && y == 2) totalScore += 50;
            for (int j = 0; j < 2; j++) {
                if (!playerBuilders[j]) continue;
                int px = playerBuilders[j]->getPosition()->getX();
                int py = playerBuilders[j]->getPosition()->getY();
                int dist = abs(x - px) + abs(y - py);
                if (dist <= 1) totalScore += 40;
            }
        }
    }
    return totalScore;
}

int Bob::countMoves(Builder* builder) {
    int moves = 0;
    Case* pos = builder->getPosition();
    int x = pos->getX(), y = pos->getY();
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            if (builder->moveBuilder(x + dx, y + dy)) {
                moves++;
                builder->undoMove();
            }
        }
    }
    return moves;
}

// =============================================================================
// MINIMAX CORRIGÉ
// BUG CRITIQUE ORIGINAL : le return après élagage alpha-beta ne appelait jamais
// undoMove(), ce qui corrompait le plateau et causait crashes/résultats erronés.
// Fix : utiliser un flag "cutoff" pour sortir des boucles internes, puis toujours
// appeler undoMove() avant de retourner.
// =============================================================================
int Bob::minimax(int depth, int alpha, int beta, bool maximizing) {
    if (depth == 0) return score();

    if (maximizing) {
        int maxEval = -100000;

        for (Builder* builder : {iaBuilderFirst, iaBuilderSecond}) {
            if (!builder) continue;

            Case* pos = builder->getPosition();
            int x = pos->getX(), y = pos->getY();

            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;

                    if (builder->moveBuilder(x + dx, y + dy)) {

                        // Victoire immédiate détectée → pas besoin de construire
                        if (builder->getPosition()->getFloor() == 3) {
                            builder->undoMove();
                            return 20000;
                        }

                        Case* newPos = builder->getPosition();
                        int nx = newPos->getX(), ny = newPos->getY();

                        bool cutoff = false;
                        for (int bx = -1; bx <= 1 && !cutoff; bx++) {
                            for (int by = -1; by <= 1 && !cutoff; by++) {
                                if (bx == 0 && by == 0) continue;

                                if (builder->createBuild(nx + bx, ny + by)) {
                                    int eval = minimax(depth - 1, alpha, beta, false);
                                    maxEval = std::max(maxEval, eval);
                                    alpha  = std::max(alpha, eval);
                                    builder->undoBuild();
                                    if (beta <= alpha) cutoff = true;
                                }
                            }
                        }

                        // TOUJOURS annuler le déplacement avant de retourner
                        builder->undoMove();
                        if (cutoff) return maxEval;
                    }
                }
            }
        }
        return (maxEval == -100000) ? -20000 : maxEval;
    }
    else {
        int minEval = 100000;

        for (Builder* builder : {playerBuilderFirst, playerBuilderSecond}) {
            if (!builder) continue;

            Case* pos = builder->getPosition();
            int x = pos->getX(), y = pos->getY();

            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;

                    if (builder->moveBuilder(x + dx, y + dy)) {

                        // Le joueur gagne → retour immédiat
                        if (builder->getPosition()->getFloor() == 3) {
                            builder->undoMove();
                            return -20000;
                        }

                        Case* newPos = builder->getPosition();
                        int nx = newPos->getX(), ny = newPos->getY();

                        bool cutoff = false;
                        for (int bx = -1; bx <= 1 && !cutoff; bx++) {
                            for (int by = -1; by <= 1 && !cutoff; by++) {
                                if (bx == 0 && by == 0) continue;

                                if (builder->createBuild(nx + bx, ny + by)) {
                                    int eval = minimax(depth - 1, alpha, beta, true);
                                    minEval = std::min(minEval, eval);
                                    beta    = std::min(beta, eval);
                                    builder->undoBuild();
                                    if (beta <= alpha) cutoff = true;
                                }
                            }
                        }

                        // TOUJOURS annuler le déplacement avant de retourner
                        builder->undoMove();
                        if (cutoff) return minEval;
                    }
                }
            }
        }
        return (minEval == 100000) ? 20000 : minEval;
    }
}

void Bob::playTurn() {
    int depth;
    if      (difficulty_ == 3) depth = 5;
    else if (difficulty_ == 2) depth = 3;
    else                       depth = 1;

    std::cout << "[IA] Bob crash ici 1.1" << std::endl;

    int bestScore = -100000;
    MoveInfo bestMove;
    Builder* bestBuilder = nullptr;

    for (Builder* builder : {iaBuilderFirst, iaBuilderSecond}) {
        std::cout << "[IA] Bob crash ici 1.2" << std::endl;
        if (!builder) continue;
        Case* pos = builder->getPosition();
        int x = pos->getX(), y = pos->getY();

        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                std::cout << "[IA] Bob crash ici 1.3" << std::endl;
                std::cout << " " << x << " " << dx << " " << y << " " << dy << std::endl;
                if (builder->moveBuilder(x + dx, y + dy)) {
                    std::cout << "[IA] Bob crash ici 1.4" << std::endl;
                    // Victoire immédiate → on joue tout de suite
                    if (builder->getPosition()->getFloor() == 3) {
                        std::cout << "[IA] Bob crash ici 1.5" << std::endl;
                        bestScore   = 20000;
                        bestMove    = {x + dx, y + dy, x + dx, y + dy, 20000};
                        bestBuilder = builder;
                        builder->undoMove();
                        goto applyMove;
                    }

                    Case* newPos = builder->getPosition();
                    int nx = newPos->getX(), ny = newPos->getY();
                    std::cout << "[IA] Bob crash ici 1.6" << std::endl;
                    for (int bx = -1; bx <= 1; bx++) {
                        for (int by = -1; by <= 1; by++) {
                            if (bx == 0 && by == 0) continue;
                            std::cout << "[IA] Bob crash ici 1.7" << std::endl;
                            if (builder->createBuild(nx + bx, ny + by)) {
                                int eval = minimax(depth - 1, -100000, 100000, false);
                                if (eval > bestScore) {
                                    bestScore   = eval;
                                    bestMove    = {nx, ny, nx + bx, ny + by, eval};
                                    bestBuilder = builder;
                                    std::cout << "[IA] Bob crash ici 1.8" << std::endl;
                                }
                                builder->undoBuild();
                            }
                        }
                    }
                    builder->undoMove();
                }
                std::cout << "[IA] Bob crash ici 1.9" << std::endl;
            }
        }
    }

applyMove:
    if (bestBuilder) {
        bestBuilder->moveBuilder(bestMove.moveX, bestMove.moveY);
        // Si victoire immédiate, pas de construction
        if (bestScore < 20000) {
            bestBuilder->createBuild(bestMove.buildX, bestMove.buildY);
        }
    }
}