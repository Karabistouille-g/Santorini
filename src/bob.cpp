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
                }
                else {
                    if (!playerBuilderFirst) playerBuilderFirst = tempBuilder;
                    else if (!playerBuilderSecond) playerBuilderSecond = tempBuilder;
                }
            }
        }
    }
}
int Bob::score() {
    // Lecture directe du plateau réel (fonctionne car minimax utilise move/undoMove)
    Builder* iaBuilders[2]     = {iaBuilderFirst, iaBuilderSecond};
    Builder* playerBuilders[2] = {playerBuilderFirst, playerBuilderSecond};

    // 1. DETECTION DE VICTOIRE
    for (int i = 0; i < 2; i++) {
        if (iaBuilders[i]->getPosition()->getFloor() == 3)     return 20000;
    }
    for (int i = 0; i < 2; i++) {
        if (playerBuilders[i]->getPosition()->getFloor() == 3) return -20000;
    }

    int totalScore = 0;

    // NIVEAU 1 : FACILE
    if (difficulty_ == 1) {
        for (int i = 0; i < 2; i++) {
            totalScore += iaBuilders[i]->getPosition()->getFloor() * 10;
        }
    }
    // NIVEAU 2 : MOYEN
    else if (difficulty_ == 2) {
        for (int i = 0; i < 2; i++) {
            int x = iaBuilders[i]->getPosition()->getX();
            int y = iaBuilders[i]->getPosition()->getY();
            int h = iaBuilders[i]->getPosition()->getFloor();
            totalScore += h * 30;
            if (x >= 1 && x <= 3 && y >= 1 && y <= 3) totalScore += 10;
        }
    }
    // NIVEAU 3 : DIFFICILE
    else {
        for (int i = 0; i < 2; i++) {
            int x = iaBuilders[i]->getPosition()->getX();
            int y = iaBuilders[i]->getPosition()->getY();
            int h = iaBuilders[i]->getPosition()->getFloor();

            totalScore += h * 100;
            if (x == 2 && y == 2) totalScore += 50;

            for (int j = 0; j < 2; j++) {
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
    int x = pos->getX();
    int y = pos->getY();

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

int Bob::minimax(int depth, int alpha, int beta, bool maximizing) {

    if (depth == 0)
        return score();

    if (maximizing) {

        int maxEval = -100000;

        for (Builder* builder : {iaBuilderFirst, iaBuilderSecond}) {

            Case* pos = builder->getPosition();
            int x = pos->getX();
            int y = pos->getY();

            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {

                    if (dx == 0 && dy == 0) continue;

                    if (builder->moveBuilder(x + dx, y + dy)) {

                        Case* newPos = builder->getPosition();
                        int nx = newPos->getX();
                        int ny = newPos->getY();

                        for (int bx = -1; bx <= 1; bx++) {
                            for (int by = -1; by <= 1; by++) {

                                if (bx == 0 && by == 0) continue;

                                if (builder->createBuild(nx + bx, ny + by)) {

                                    int eval = minimax(depth - 1, alpha, beta, false);

                                    maxEval = std::max(maxEval, eval);
                                    alpha = std::max(alpha, eval);

                                    builder->undoBuild();

                                    if (beta <= alpha)
                                        return maxEval;
                                }
                            }
                        }

                        builder->undoMove();
                    }
                }
            }
        }

        return maxEval;
    }
    else {

        int minEval = 100000;

        for (Builder* builder : {playerBuilderFirst, playerBuilderSecond}) {

            Case* pos = builder->getPosition();
            int x = pos->getX();
            int y = pos->getY();

            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {

                    if (dx == 0 && dy == 0) continue;

                    if (builder->moveBuilder(x + dx, y + dy)) {

                        Case* newPos = builder->getPosition();
                        int nx = newPos->getX();
                        int ny = newPos->getY();

                        for (int bx = -1; bx <= 1; bx++) {
                            for (int by = -1; by <= 1; by++) {

                                if (bx == 0 && by == 0) continue;

                                if (builder->createBuild(nx + bx, ny + by)) {

                                    int eval = minimax(depth - 1, alpha, beta, true);

                                    minEval = std::min(minEval, eval);
                                    beta = std::min(beta, eval);

                                    builder->undoBuild();

                                    if (beta <= alpha)
                                        return minEval;
                                }
                            }
                        }

                        builder->undoMove();
                    }
                }
            }
        }

        return minEval;
    }
}

void Bob::playTurn() {
    int depth;

    // Définition de la profondeur selon la difficulté
    if (difficulty_ == 1) {
        depth = 1; // Niveau 1 : Coup immédiat
    } else if (difficulty_ == 2) {
        depth = 3; // Niveau 2 : Anticipe un peu
    } else {
        depth = 5; // Niveau 3 : anticipe 5 coups à l'avance
    }

    int bestScore = -100000;
    MoveInfo bestMove;
    Builder* bestBuilder = nullptr;

    // On parcourt les deux bâtisseurs de l'IA
    for (Builder* builder : {iaBuilderFirst, iaBuilderSecond}) {
        Case* pos = builder->getPosition();
        int x = pos->getX();
        int y = pos->getY();

        // Simulation du Déplacement
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;

                if (builder->moveBuilder(x + dx, y + dy)) {
                    Case* newPos = builder->getPosition();
                    int nx = newPos->getX();
                    int ny = newPos->getY();

                    // Simulation de la Construction
                    for (int bx = -1; bx <= 1; bx++) {
                        for (int by = -1; by <= 1; by++) {
                            if (bx == 0 && by == 0) continue;

                            if (builder->createBuild(nx + bx, ny + by)) {
                                
                                // Appel du minimax avec élagage Alpha-Beta pour tenir la profondeur 5+
                                // On commence à depth - 1 car on vient de simuler le premier coup
                                int eval = minimax(depth - 1, -100000, 100000, false);

                                if (eval > bestScore) {
                                    bestScore = eval;
                                    bestMove = {nx, ny, nx + bx, ny + by, eval};
                                    bestBuilder = builder;
                                }

                                builder->undoBuild(); // Annule construction
                            }
                        }
                    }
                    builder->undoMove(); // Annule déplacement
                }
            }
        }
    }

    // Application réelle du meilleur coup trouvé sur le plateau
    if (bestBuilder) {
        bestBuilder->moveBuilder(bestMove.moveX, bestMove.moveY);
        bestBuilder->createBuild(bestMove.buildX, bestMove.buildY);
    }
}