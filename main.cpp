#include <iostream>
#include <cmath>
#include "pieces.h"
#include <chrono>

using namespace std::chrono;

/**
 * Orientation of the piece, A is the standard orientation
 */
enum class Orientation {
    A, B, C, D
};

/**
 * Side of the piece in Orientation A
 */
enum class Side {
    TOP, RIGHT, DOWN, LEFT
};


/**
 * Check if an attachment can be next to another
 * @param a A first  AttachementType to check
 * @param b A second AttachementType to check
 * @return true if compatible, false otherwise
 */
bool isCompatible(AttachementType a, AttachementType b) {
    switch (a) {
        case FILLE_HAUT:
            return b == FILLE_BAS;
        case FILLE_BAS:
            return b == FILLE_HAUT;
        case DAME_HAUT:
            return b == DAME_BAS;
        case DAME_BAS:
            return b == DAME_HAUT;
        case ARROSOIR_GAUCHE:
            return b == ARROSOIR_DROIT;
        case ARROSOIR_DROIT:
            return b == ARROSOIR_GAUCHE;
        case GATEAU_GAUCHE:
            return b == GATEAU_DROIT;
        case GATEAU_DROIT:
            return b == GATEAU_GAUCHE;
        case ARROSOIR_INVERSE:
        case NONE:
        default:
            return false;
    }
}

/**
 * A piece of the puzzle
 * consist of a number, a position, an orientation and 4 attachments
 */
class PuzzlePiece {
public:
    PuzzlePiece(Piece list, int number);

    AttachementType getSide(Side side, Orientation orientation) const;

    void setOrientation(Orientation orientation1);

    bool canBeNeighbour(const PuzzlePiece &piece) const;

    int getPosition() const;

    void setPosition(int position);

    std::string getOrientationName() const;

    std::string toString() const;


private:
    int position;
    const int number;
    AttachementType attachement[4];
    Orientation orientation;
};

PuzzlePiece::PuzzlePiece(Piece list, int number) : number(number), position(-1),
                                                   orientation(Orientation::A) {
    attachement[0]=list.at(0);
    attachement[1]=list.at(1);
    attachement[2]=list.at(2);
    attachement[3]=list.at(3);
}

inline AttachementType PuzzlePiece::getSide(Side side, Orientation orientation) const {
    return attachement[(((int) orientation+(int) side)%4)];
}

bool PuzzlePiece::canBeNeighbour(const PuzzlePiece &piece) const {
    int d = position - piece.position;
    Side ownSide;
    Side pieceSide;
    if (abs(d) == 1 && std::floor((piece.position - 1) / 3) == std::floor((position - 1) / 3)) {
        if (d == 1) {
            ownSide = Side::LEFT;
            pieceSide = Side::RIGHT;
        } else {
            ownSide = Side::RIGHT;
            pieceSide = Side::LEFT;
        }
    } else if (abs(d) == 3) {
        if (d == 3) {
            ownSide = Side::TOP;
            pieceSide = Side::DOWN;
        } else {
            ownSide = Side::DOWN;
            pieceSide = Side::TOP;
        }
    } else {
        return false;
    }
    return isCompatible(this->getSide(ownSide, this->orientation), piece.getSide(pieceSide, piece.orientation));
}

void PuzzlePiece::setOrientation(Orientation orientation) {
    this->orientation = orientation;
}

int PuzzlePiece::getPosition() const {
    return position;
}

void PuzzlePiece::setPosition(int position) {
    this->position = position;
}


std::string PuzzlePiece::getOrientationName() const {
    std::string output;
    switch (orientation) {
        case Orientation::A:
            output = "a";
            break;
        case Orientation::B:
            output = "b";
            break;
        case Orientation::C:
            output = "c";
            break;
        case Orientation::D:
            output = "d";
            break;
    }
    return output;
}

std::string PuzzlePiece::toString() const {
    return std::to_string(number) + getOrientationName();
}


PuzzlePiece *getPieceArPosition(std::vector<PuzzlePiece> &list, int position) {
    for (auto &i : list) {
        if (i.getPosition() == position) {
            return &i;
        }
    }
    return nullptr;
}

/**
 *
 * @param position
 * @return a vector with the position of the adjacent pieces
 */
std::vector<int> getAllAdjacent(int position) {
    std::vector<int> output;
    --position;
    for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; ++j) {
            if (abs(i) == abs(j)) {
                continue;
            }
            if (i == -1 and position % 3 == 0) {
                continue;
            }
            if (i == 1 and position % 3 == 2) {
                continue;
            }
            if (j == -1 and floor(position / 3) == 0) {
                continue;
            }
            if (j == 1 and floor(position / 3) == 2) {
                continue;
            }
            int test = position + i + j * 3;
            if (test >= 0 && test < 9) {
                output.push_back(test + 1);
            }
        }
    }
    return output;
}


std::vector<Orientation> possibleOrientation(std::vector<PuzzlePiece> &list, PuzzlePiece &piece) {
    std::vector<int> neighbours = getAllAdjacent(piece.getPosition());
    std::vector<Orientation> output;

    for (Orientation orientation = Orientation::A;
         orientation <= Orientation::D; orientation = (Orientation) ((int) orientation + 1)) {
        bool isValid = true;
        for (int n : neighbours) {
            PuzzlePiece *pie = getPieceArPosition(list, n);
            if (pie == nullptr) {
                continue;
            }
            int p = pie->getPosition();
            if (p == n) {
                piece.setOrientation(orientation);
                isValid = isValid && piece.canBeNeighbour(*pie);
            }
        }
        if (isValid) {
            output.push_back(orientation);
        }
    }
    return output;
}

bool solution(std::vector<PuzzlePiece> &list, int position) {

    std::vector<PuzzlePiece *> free;
    free.reserve(list.size() - position + 1);
    for (PuzzlePiece &p : list) {
        if (p.getPosition() == -1) {
            free.push_back(&p);
        }
    }
    free.shrink_to_fit();
    bool hasPossibilities = !free.empty();
    if (!hasPossibilities) {
        return false;
    }
    // last one to avoid moving elements
    PuzzlePiece *current = free.back();

    while (hasPossibilities) {
        current->setPosition(position);
        std::vector<Orientation> PossibleOrientation = possibleOrientation(list, *current);
        for (Orientation orientation : PossibleOrientation) {
            current->setOrientation(orientation);
            if (position == list.size()) {
                for (int k = 0; k < list.size(); ++k) {
                    for (const PuzzlePiece p : list) {
                        if (p.getPosition() != k) {
                            continue;
                        }
                        std::cout << p.toString() << " ";
                    }
                }
                std::cout << std::endl;
            } else {
                if (solution(list, position + 1)) {
                    return true;
                }
            }
        }

        current->setPosition(-1);
        free.pop_back();

        hasPossibilities = !free.empty();
        if (!hasPossibilities) {
            return false;
        }
        current = free.back();
    }
    return false;
}

int main() {
    std::vector<PuzzlePiece> list;
    int nb = 1;
    for (Piece p : PIECES) {
        list.emplace_back(p, nb++);
    }

    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    solution(list, 1);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
//calcul du temps, ici en nanosecondes
    double temps = duration_cast<nanoseconds>(t2 - t1).count();
    std::cout << temps << std::endl;
    /*for (const std::string &s : l) {
        std::cout << s << std::endl;
    }
    std::cout << "total " << l.size() << std::endl << std::endl;*/


}
