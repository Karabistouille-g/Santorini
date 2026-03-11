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
        Board* board = Board::getInstance();
        for (int i = 0; i < 2; i++) {
            if (!iaBuilders[i]) continue;
            int x = iaBuilders[i]->getPosition()->getX();
            int y = iaBuilders[i]->getPosition()->getY();
            int h = iaBuilders[i]->getPosition()->getFloor();
            totalScore += h * 50;
            if (x >= 1 && x <= 3 && y >= 1 && y <= 3) totalScore += 15;

            // Bonus sur les bâtiments adjacents :
            // L'IA doit valoriser les tours hautes PRÈS D'ELLE,
            // car c'est ce qu'elle va gravir pour gagner.
            //   adj lvl 1 libre → +15  (matériau de base, peut devenir lvl 2)
            //   adj lvl 2 libre → +60  (peut construire lvl 3 dessus)
            //   adj lvl 2 libre + elle est lvl 1 → +150  (plan sur 2 coups : build lvl3 puis monte)
            //   adj lvl 3 libre + elle est lvl 2 → +500  (menace de victoire immédiate)
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = x + dx, ny = y + dy;
                    if (nx < 0 || nx >= 5 || ny < 0 || ny >= 5) continue;
                    Case* nc = board->getCase(nx, ny);
                    int   nf = nc->getFloor();
                    bool  free = (nc->getBuilder() == nullptr);
                    if (!free) continue;

                    if (nf == 1) totalScore += 15;
                    if (nf == 2) totalScore += 60;
                    if (nf == 2 && h == 1) totalScore += 150;
                    if (nf == 3 && h == 2) totalScore += 500;
                }
            }

            // Bloquer l'adversaire s'il est sur niveau 2
            for (int j = 0; j < 2; j++) {
                if (!playerBuilders[j]) continue;
                int px = playerBuilders[j]->getPosition()->getX();
                int py = playerBuilders[j]->getPosition()->getY();
                int ph = playerBuilders[j]->getPosition()->getFloor();
                if (ph == 2) {
                    int dist = std::max(abs(x - px), abs(y - py));
                    if (dist <= 1) totalScore += 80;
                }
            }
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
// MOVE ORDERING : score rapide pour trier les destinations avant de recurser.
// =============================================================================
int Bob::quickEval(int nx, int ny, bool maximizing) {
    Case* target = b->getCase(nx, ny);
    if (!target) return 0;
    int s = target->getFloor() * (maximizing ? 30 : -30);
    if (maximizing  && target->getFloor() == 3) return 9000;
    if (!maximizing && target->getFloor() == 3) return -9000;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            int ax = nx+dx, ay = ny+dy;
            if (ax < 0 || ax >= 5 || ay < 0 || ay >= 5) continue;
            Case* adj = b->getCase(ax, ay);
            if (adj && !adj->getBuilder())
                s += adj->getFloor() * (maximizing ? 8 : -8);
        }
    }
    return s;
}

// =============================================================================
// MINIMAX — boucles imbriquées classiques + move ordering sur les destinations.
//
// POURQUOI ce n'est plus une liste plate :
// L'ancienne version générait (move, build) en une seule liste et appelait
// moveBuilder plusieurs fois pour la même destination. Si une iteration
// laissait la stack moves_ dans un état inattendu, les appels suivants
// à undoMove pouvaient vider la stack → top() sur stack vide → UB →
// position_ corrompu → segfault dans validCase.
//
// Ici : un seul moveBuilder/undoMove par destination, les builds sont
// traités dans la boucle intérieure. La stack est toujours équilibrée.
// =============================================================================
int Bob::minimax(int depth, int alpha, int beta, bool maximizing) {
    if (depth == 0) return score();

    Builder* iaArr[2]     = {iaBuilderFirst,     iaBuilderSecond};
    Builder* playerArr[2] = {playerBuilderFirst, playerBuilderSecond};
    Builder** builders    = maximizing ? iaArr : playerArr;

    // --- Collecter et trier les destinations (move ordering) ---
    struct Dest { Builder* builder; int nx, ny; int h; };
    std::vector<Dest> dests;
    dests.reserve(32);

    for (int bi = 0; bi < 2; bi++) {
        Builder* builder = builders[bi];
        if (!builder) continue;
        int x        = builder->getPosition()->getX();
        int y        = builder->getPosition()->getY();
        int curFloor = builder->getPosition()->getFloor();

        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                int nx = x+dx, ny = y+dy;
                if (nx < 0 || nx >= 5 || ny < 0 || ny >= 5) continue;
                Case* mc = b->getCase(nx, ny);
                if (!mc || mc->getBuilder() || mc->getFloor() >= 4) continue;
                if (mc->getFloor() > curFloor + 1) continue;
                dests.push_back({builder, nx, ny, quickEval(nx, ny, maximizing)});
            }
        }
    }

    if (dests.empty()) return maximizing ? -20000 : 20000;

    if (maximizing)
        std::sort(dests.begin(), dests.end(), [](const Dest& a, const Dest& b){ return a.h > b.h; });
    else
        std::sort(dests.begin(), dests.end(), [](const Dest& a, const Dest& b){ return a.h < b.h; });

    int best = maximizing ? -100000 : 100000;

    for (auto& dest : dests) {
        // --- UN SEUL moveBuilder par destination ---
        if (!dest.builder->moveBuilder(dest.nx, dest.ny)) continue;

        // Victoire immédiate
        if (dest.builder->getPosition()->getFloor() == 3) {
            dest.builder->undoMove();
            return maximizing ? 20000 : -20000;
        }

        // --- Boucle interne sur les constructions ---
        bool cutoff = false;
        for (int bx = -1; bx <= 1 && !cutoff; bx++) {
            for (int by = -1; by <= 1 && !cutoff; by++) {
                if (bx == 0 && by == 0) continue;
                int cbx = dest.nx+bx, cby = dest.ny+by;
                if (cbx < 0 || cbx >= 5 || cby < 0 || cby >= 5) continue;
                if (!dest.builder->createBuild(cbx, cby)) continue;

                int eval = minimax(depth - 1, alpha, beta, !maximizing);
                dest.builder->undoBuild();

                if (maximizing) { best = std::max(best, eval); alpha = std::max(alpha, eval); }
                else            { best = std::min(best, eval); beta  = std::min(beta,  eval); }
                if (beta <= alpha) cutoff = true;
            }
        }

        // --- UN SEUL undoMove, toujours appelé ---
        dest.builder->undoMove();

        if (cutoff) break;
    }
    return best;
}

void Bob::playTurn() {
    int depth;
    if      (difficulty_ == 3) depth = 4;
    else if (difficulty_ == 2) depth = 3;
    else                       depth = 1;

    // === SNAPSHOT du plateau avant le minimax ===
    // Le minimax modifie/restaure le board via moveBuilder/undoMove etc.
    // Si un undo rate silencieusement (guard), le board se corrompt progressivement.
    // On sauvegarde l'etat complet et on le restaure de force apres la recherche.
    Case* snap_ia1  = iaBuilderFirst  ? iaBuilderFirst->getPosition()  : nullptr;
    Case* snap_ia2  = iaBuilderSecond ? iaBuilderSecond->getPosition() : nullptr;
    Case* snap_p1   = playerBuilderFirst  ? playerBuilderFirst->getPosition()  : nullptr;
    Case* snap_p2   = playerBuilderSecond ? playerBuilderSecond->getPosition() : nullptr;

    int snapFloors[5][5];
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            snapFloors[i][j] = b->getCase(i, j)->getFloor();
    // =============================================

    int bestScore = -100000;
    MoveInfo bestMove{};
    Builder* bestBuilder = nullptr;

    for (Builder* builder : {iaBuilderFirst, iaBuilderSecond}) {
        if (!builder) continue;
        int x = builder->getPosition()->getX();
        int y = builder->getPosition()->getY();

        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue;
                if (!builder->moveBuilder(x + dx, y + dy)) continue;

                if (builder->getPosition()->getFloor() == 3) {
                    int nx = builder->getPosition()->getX();
                    int ny = builder->getPosition()->getY();
                    bestMove    = {nx, ny, nx, ny, 20000};
                    bestBuilder = builder;
                    bestScore   = 20000;
                    builder->undoMove();
                    goto applyMove;
                }

                int nx = builder->getPosition()->getX();
                int ny = builder->getPosition()->getY();

                for (int bx = -1; bx <= 1; bx++) {
                    for (int by = -1; by <= 1; by++) {
                        if (bx == 0 && by == 0) continue;
                        int cbx2 = nx+bx, cby2 = ny+by;
                        if (cbx2 < 0 || cbx2 >= 5 || cby2 < 0 || cby2 >= 5) continue;
                        if (!builder->createBuild(cbx2, cby2)) continue;
                        int eval = minimax(depth - 1, -100000, 100000, false);
                        if (eval > bestScore) {
                            bestScore   = eval;
                            bestMove    = {nx, ny, nx+bx, ny+by, eval};
                            bestBuilder = builder;
                        }
                        builder->undoBuild();
                    }
                }
                builder->undoMove();
            }
        }
    }

applyMove:
    // === RESTORE du plateau apres le minimax ===
    // On remet le board exactement dans l'etat d'avant la recherche,
    // puis on applique le meilleur coup trouve proprement.
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++) {
            Case* c = b->getCase(i, j);
            c->setBuilder(nullptr);
            while (c->getFloor() > snapFloors[i][j]) c->removeFloor();
            while (c->getFloor() < snapFloors[i][j]) c->addFloor();
        }
    if (iaBuilderFirst  && snap_ia1) iaBuilderFirst->hardReset(snap_ia1);
    if (iaBuilderSecond && snap_ia2) iaBuilderSecond->hardReset(snap_ia2);
    if (playerBuilderFirst  && snap_p1) playerBuilderFirst->hardReset(snap_p1);
    if (playerBuilderSecond && snap_p2) playerBuilderSecond->hardReset(snap_p2);
    // =============================================

    if (bestBuilder) {
    bool moved = bestBuilder->moveBuilder(bestMove.moveX, bestMove.moveY);
    
    // On vérifie l'état réel du plateau : si l'IA n'est pas sur un étage 3, elle DOIT construire.
    if (moved && bestBuilder->getPosition()->getFloor() != 3) {
        bestBuilder->createBuild(bestMove.buildX, bestMove.buildY);
    }
}
}bool Builder::createBuild(int x, int y) {
    if (x < 0 || x >= 5 || y < 0 || y >= 5) return false;
    std::cout << getPosition()->getX() << "," << getPosition()->getY() << " -> " << x << "," << y << std::endl;

    Case* target = b_->getCase(x, y);
    if (!validCase(target)) return false;

    std::cout << "Target floor before build: " << target->getFloor() << std::endl;

    if (target->getFloor() >= 4) return false;
    if (target->getBuilder()) return false;

    std::cout << "Build successful" << std::endl;

    target->addFloor();
    builds_.push(target);
    return true;
}