#include <iostream>
#include <cmath>
#include "pieces.h"

enum class Orientation {
    A, B, C, D
};

enum class Side {
    TOP, RIGHT, DOWN, LEFT
};


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

class PuzzlePiece {
public:
    PuzzlePiece(Piece list, int number);

    AttachementType getSide(Side side, Orientation orientation) const;

    void setOrientation(Orientation orientation1);

    bool canBeNeighbour(const PuzzlePiece &piece) const;

    int getPosition() const;

    void setPosition(int position);

    int getNumber() const;

    Orientation getOrientation() const;

    std::string getOrientationName() const;

    int getPosInList() const;

    void setPosInList(int posInList);

private:
    int position;
    AttachementType top;
    AttachementType right;
    AttachementType down;
    AttachementType left;
    int number;
    Orientation orientation;
    int posInList;
};

PuzzlePiece::PuzzlePiece(Piece list, int number) : top(list.at(0)), right(list.at(1)), down(list.at(2)),
                                                   left(list.at(3)), number(number), position(-1),
                                                   orientation(Orientation::A), posInList(number - 1) {}

AttachementType PuzzlePiece::getSide(Side side, Orientation orientation) const {
    switch (orientation) {
        case Orientation::A:
            switch (side) {
                case Side::TOP:
                    return top;
                case Side::RIGHT:
                    return right;
                case Side::DOWN:
                    return down;
                case Side::LEFT:
                    return left;
            }
        case Orientation::B:
            switch (side) {
                case Side::TOP:
                    return right;
                case Side::RIGHT:
                    return down;
                case Side::DOWN:
                    return left;
                case Side::LEFT:
                    return top;
            }
        case Orientation::C:
            switch (side) {
                case Side::TOP:
                    return down;
                case Side::RIGHT:
                    return left;
                case Side::DOWN:
                    return top;
                case Side::LEFT:
                    return right;
            }
        case Orientation::D:
            switch (side) {
                case Side::TOP:
                    return left;
                case Side::RIGHT:
                    return top;
                case Side::DOWN:
                    return right;
                case Side::LEFT:
                    return down;
            }
    }
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

int PuzzlePiece::getNumber() const {
    return number;
}

Orientation PuzzlePiece::getOrientation() const {
    return orientation;
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

int PuzzlePiece::getPosInList() const {
    return posInList;
}

void PuzzlePiece::setPosInList(int posInList) {
    PuzzlePiece::posInList = posInList;
}

PuzzlePiece *getPieceArPosition(std::vector<PuzzlePiece> &list, int position) {
    for (auto &i : list) {
        if (i.getPosition() == position) {
            return &i;
        }
    }
    return nullptr;
}

void displayList(const std::vector<PuzzlePiece> &list) {
    for (PuzzlePiece p : list) {
        std::cout << p.getNumber() << " " << p.getPosition() << " " << p.getOrientationName() << " ";
    }
    std::cout << std::endl;
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
                Orientation prevOr = piece.getOrientation();
                piece.setOrientation(orientation);
                isValid = isValid && piece.canBeNeighbour(*pie);
                piece.setOrientation(prevOr);

            }
        }
        if (isValid) {
            output.push_back(orientation);
        }
    }
    return output;
}

int getEmptyPositionCount(const std::vector<PuzzlePiece> &list) {
    int output = 0;
    for (const auto &i : list) {
        if (i.getPosition() == -1) {
            ++output;
        }
    }
    return output;
}

PuzzlePiece *permutation(std::vector<PuzzlePiece> &list, int indexA, int indexB) {
    if (indexA == indexB) {
        return &list.at(indexA);
    }
    PuzzlePiece temp = list.at(indexA);
    list.at(indexA) = list.at(indexB);
    list.at(indexA).setPosInList(indexA);
    list.at(indexB) = temp;
    list.at(indexB).setPosInList(indexB);
    return &list.at(indexA);
}

void permutationElement(std::vector<PuzzlePiece> &list, int indexA, int indexB) {
    if (indexA == indexB) {
        return;
    }
    PuzzlePiece temp = list.at(indexA);
    list.at(indexA) = list.at(indexB);
    list.at(indexA).setPosInList(indexA);
    list.at(indexB) = temp;
    list.at(indexB).setPosInList(indexB);
}

bool solution(std::vector<PuzzlePiece> &list, int position) {
    static int tries;
    static int nbPosition;
    if (position == 1) {
        tries = 0;
    }
    if (position == 6) {
        ++tries;
        --tries;
    }
    ++tries;
    if (position == list.size()+1) {
        return true;
    }
    std::vector<PuzzlePiece *> free;
    free.reserve(9);
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
    PuzzlePiece *current = free.at(0);

    while (hasPossibilities) {
        current->setPosition(position);

        std::vector<Orientation> PossibleOrientation = possibleOrientation(list, *current);
        for (Orientation orientation : PossibleOrientation) {
            current->setOrientation(orientation);
            if (getEmptyPositionCount(list) == 0) {
                return true;
            } else {
                bool test = solution(list, position + 1);
                if (test) {
                    return true;
                }
            }
        }

        current->setPosition(-1);
        std::vector<PuzzlePiece *> free2 = free;
        free.clear();
        for (auto &i : free2) {
            if (i->getNumber() != current->getNumber()) {
                free.push_back(i);
            }
        }
        hasPossibilities = !free.empty();
        if (!hasPossibilities) {
            return false;
        }
        current = free.at(0);
    }
    return false;


}

int main() {
    std::vector<PuzzlePiece> list;
    int nb = 1;
    for (Piece p : PIECES) {
        list.emplace_back(p, nb++);
    }
    for (int i = 1; i <= 9; ++i) {
        permutationElement(list, i - 1, 8);
        solution(list, 1);
        for (PuzzlePiece k : list) {
            std::cout << k.getPosition()<<":" <<k.getNumber() << k.getOrientationName() << " ";
        }
        permutationElement(list, i - 1, 8);

        std::cout << std::endl;
    }


    for (PuzzlePiece k : list) {
        std::cout << k.getNumber() << ":" << k.getPosition() << k.getOrientationName() << std::endl;
    }

}
