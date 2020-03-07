#include <iostream>
#include <cmath>
#include "pieces.h"
#include <chrono>

using namespace std::chrono;

// Number of piece per line and columns
const int size = 3;

/**
 * Orientation of the piece, A is the standard orientation
 */
enum class Orientation {
    A, B, C, D
};

/**
  * Get the name of the orientation in a human readable format
  * @return a string containing the name of the orientation
  */
std::string getOrientationName(Orientation orientation);

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
    /**
     * Create a piece with a number and attachments
     * @details position is initialized at -1 and orientation at A
     * @param list a vector of AttachementType
     * @param number the unique number of the list
     * @attention no verification is made for the uniqueness of the number
     */
    PuzzlePiece(Piece list, int number);

    /**
     * Get the AttachementType on a given size based on the current orientation
     * @param side The side of the attachement to get with the current orientation
     * @return an AttachementType
     * @example (TOP,B) will return the attachementType that was on the right side in orientation A
     */
    AttachementType getAttachementTypeOnSide(Side side) const;

    /**
     * Set the orientation of the piece
     */
    void setOrientation(Orientation orientation1);

    /**
     * Verify if a piece can be a neighbour to another piece based on their compatibility
     * @param piece the piece to verify the compatibility with
     * @return true if the two piece can be neighbour, false otherwise
     */
    bool canBeNeighbour(const PuzzlePiece &piece) const;

    /**
     * Get the position of the piece
     * @return an integer with the position
     */
    int getPosition() const;

    /**
     * Set the position of the piece
     * @param position the new position of the piece
     */
    void setPosition(int position);

    /**
     * Get a description of the piece in a human readable format
     * @return a string with the number of the piece and its orientation
     */
    std::string toString() const;

private:
    int position;
    int number;
    AttachementType attachement[4]{};
    Orientation orientation;
};

PuzzlePiece::PuzzlePiece(Piece list, int number) : number(number), position(-1),
                                                   orientation(Orientation::A) {
    attachement[0] = list.at(0);
    attachement[1] = list.at(1);
    attachement[2] = list.at(2);
    attachement[3] = list.at(3);
}

inline AttachementType PuzzlePiece::getAttachementTypeOnSide(Side side) const {
    return attachement[(((int) orientation + (int) side) % 4)];
}

bool PuzzlePiece::canBeNeighbour(const PuzzlePiece &piece) const {
    Side ownSide;
    Side pieceSide;
    int x = (position - 1) % size - (piece.position - 1) % size;
    int y = (position - 1) / size - (piece.position - 1) / size;
    if (x == y) {
        return false;
    } else if (x == -1) {
        ownSide = Side::RIGHT;
        pieceSide = Side::LEFT;
    } else if (x == 1) {
        ownSide = Side::LEFT;
        pieceSide = Side::RIGHT;
    } else if (y == -1) {
        ownSide = Side::DOWN;
        pieceSide = Side::TOP;
    } else if (y == 1) {
        ownSide = Side::TOP;
        pieceSide = Side::DOWN;
    }
    return isCompatible(this->getAttachementTypeOnSide(ownSide), piece.getAttachementTypeOnSide(pieceSide));
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


std::string getOrientationName(Orientation orientation) {
    switch (orientation) {
        case Orientation::A:
            return "a";
        case Orientation::B:
            return "b";
        case Orientation::C:
            return "c";
        case Orientation::D:
            return "d";
    }

}

std::string PuzzlePiece::toString() const {
    return std::to_string(number) + getOrientationName(orientation);
}

/**
 * Get the position of adjacent
 * @param position a integer for a position
 * @return a vector with the position of the adjacent pieces
 */
std::vector<int> getAllAdjacent(int position) {
    std::vector<int> output;
    --position;
    int x = position % size;
    int y = position / size;

    if (x - 1 >= 0) {
        output.push_back(x + y * size);
    }
    if (x + 1 < size) {
        output.push_back(x + 2 + y * size);
    }
    if (y - 1 >= 0) {
        output.push_back(x + 1 + (y - 1) * size);
    }
    if (y + 1 < size) {
        output.push_back(x + 1 + (y + 1) * size);
    }
    return output;
}

/**
 * Get a list of the possible orientation for a given piece depending of the piece already present on the board
 * @param list a vector of puzzlePiece containing the puzzle
 * @param piece the piece to test
 * @return a vector of orientation. Is empty if the piece doesn't match at all
 */
std::vector<Orientation> getValidOrientation(std::vector<PuzzlePiece> &list, PuzzlePiece &piece) {
    std::vector<int> neighbours = getAllAdjacent(piece.getPosition());
    std::vector<Orientation> output;

    for (Orientation orientation = Orientation::A;
         orientation <= Orientation::D; orientation = (Orientation) ((int) orientation + 1)) {
        bool isValid = true;

        for (int neighbour : neighbours) {
            PuzzlePiece &test = list.at(neighbour - 1);

            if (test.getPosition() == -1) {
                continue;
            }

            piece.setOrientation(orientation);
            isValid = isValid && piece.canBeNeighbour(test);
        }

        if (isValid) {
            output.push_back(orientation);
        }
    }
    return output;
}

/**
 * Permute two element between them
 * @param list a vector of a puzzlePiece containg the element to swap
 * @param indexA an integer the index of the first element to swap
 * @param indexB an integer the index of the second element to swap
 * @return a pointer to the new element at indexA
 */
PuzzlePiece *permutationElement(std::vector<PuzzlePiece> &list, int indexA, int indexB) {
    if (indexA == indexB) {
        return &list.at(indexA);
    }
    PuzzlePiece temp = list.at(indexA);
    list.at(indexA) = list.at(indexB);
    list.at(indexB) = temp;
    return &list.at(indexA);
}

/**
 * Solve and display all the solution of the puzzle
 * @param list a vector of puzzlePiece containing the puzzle
 * @param position the position of the puzzle to test (default is one for the starting position)
 * @return a boolean (only used in the recursion to indicate if a path is impossible)
 */
bool solution(std::vector<PuzzlePiece> &list, int position = 1) {
    //All possible piece for this position
    std::vector<PuzzlePiece *> possiblePiece;
    possiblePiece.reserve(list.size() - position + 1);
    for (int i = position - 1; i < list.size(); ++i) {
        possiblePiece.push_back(&list.at(i));
    }

    // last one to avoid moving elements
    PuzzlePiece *current = possiblePiece.back();
    // number of tries for this piece used to avoid permuting two wrong element
    int tries = 0;

    while (true) {
        current->setPosition(position);
        // Put piece tested at the corresponding position in the array
        current = permutationElement(list, position - 1, list.size() - 1 - tries);

        std::vector<Orientation> validOrientation = getValidOrientation(list, *current);
        for (Orientation orientation : validOrientation) {
            current->setOrientation(orientation);
            // If last piece display solution
            if (position == list.size()) {
                for (const PuzzlePiece p : list) {
                    std::cout << p.toString() << " ";
                }
                std::cout << std::endl;
            } else {
                solution(list, position + 1);
            }
        }

        current->setPosition(-1);
        // Put piece tested at the end of the array but before previously tested piece for this position
        permutationElement(list, position - 1, list.size() - 1 - tries);
        ++tries;
        // remove piece tested
        possiblePiece.pop_back();

        if (possiblePiece.empty()) {
            return false;
        }
        current = possiblePiece.back();
    }
}

int main() {
    std::vector<PuzzlePiece> list;
    int nb = 1;
    for (Piece p : PIECES) {
        list.emplace_back(p, nb++);

    }


    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    solution(list);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    double temps = duration_cast<nanoseconds>(t2 - t1).count();
    std::cout << temps / 1000000 << std::endl;


}
