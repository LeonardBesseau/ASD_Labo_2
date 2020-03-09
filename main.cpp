#include <iostream>
#include "pieces.h"
#include <chrono>

using namespace std::chrono;

// Number of piece per line and columns
const int size = 3;

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
        default:
            return false;
    }
}

/**
 * A piece of the puzzle
 * consist of a number, a position, an orientation and 4 attachments
 */
class PuzzlePiece {
    friend std::ostream &operator<<(std::ostream &lhs, const PuzzlePiece &rhs);

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
    void setOrientation(char orientation);

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

private:
    int position;
    int number;
    Piece attachement;
    char orientation;
};

PuzzlePiece::PuzzlePiece(Piece list, int number) : attachement(list), number(number), position(-1),
                                                   orientation('a') {}

AttachementType PuzzlePiece::getAttachementTypeOnSide(Side side) const {
    return attachement.at(((orientation - 'a' + (int) side) % 4));
}

bool PuzzlePiece::canBeNeighbour(const PuzzlePiece &piece) const {
    Side ownSide;
    Side pieceSide;
    int x = (position - 1) % size - (piece.position - 1) % size;
    int y = (position - 1) / size - (piece.position - 1) / size;

    if (x == y) { // No diagonals
        return false;
    } else if (x == -1) { // Right
        ownSide = Side::RIGHT;
        pieceSide = Side::LEFT;
    } else if (x == 1) { // Left
        ownSide = Side::LEFT;
        pieceSide = Side::RIGHT;
    } else if (y == -1) { // DOWN
        ownSide = Side::DOWN;
        pieceSide = Side::TOP;
    } else if (y == 1) { // UP
        ownSide = Side::TOP;
        pieceSide = Side::DOWN;
    } else {
        return false;
    }
    return isCompatible(this->getAttachementTypeOnSide(ownSide), piece.getAttachementTypeOnSide(pieceSide));
}

void PuzzlePiece::setOrientation(char orientation) {
    this->orientation = orientation;
}

int PuzzlePiece::getPosition() const {
    return position;
}

void PuzzlePiece::setPosition(int position) {
    this->position = position;
}

std::ostream &operator<<(std::ostream &lhs, const PuzzlePiece &rhs) {
    return lhs << rhs.number << rhs.orientation;
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
 * @param neighboursPosition a vector of vector containing the position of adjacent piece for each piece
 * @param piece the piece to test
 * @return a vector of orientation. Is empty if the piece doesn't match at all
 */
std::vector<char>
getValidOrientation(std::vector<PuzzlePiece> &list, const std::vector<std::vector<int>> &neighboursPosition,
                    PuzzlePiece &piece) {
    std::vector<int> neighbours = neighboursPosition.at(piece.getPosition() - 1);
    std::vector<char> output;

    for (char orientation = 'a'; orientation <= 'd'; ++orientation) {
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
 * Solve and display all the solution of the puzzle
 * @param list a vector of puzzlePiece containing the puzzle
 * @param neighboursPosition a vector of vector containing the position of adjacent piece for each piece
 * @param position the position of the puzzle to test (default is one for the starting position)
 * @return a boolean (only used in the recursion to indicate if a path is impossible)
 */
bool
solution(std::vector<PuzzlePiece> &list, const std::vector<std::vector<int>> &neighboursPosition, int position = 1) {
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
        std::swap(list.at(position - 1), list.at(list.size() - 1 - tries));
        current = &list.at(position - 1);

        std::vector<char> validOrientation = getValidOrientation(list, neighboursPosition, *current);
        for (char orientation : validOrientation) {
            current->setOrientation(orientation);
            // If last piece display solution
            if (position == list.size()) {
                for (const PuzzlePiece p : list) {
                    std::cout << p << " ";
                }
                std::cout << std::endl;
            } else {
                solution(list, neighboursPosition, position + 1);
            }
        }

        current->setPosition(-1);
        // Put piece tested at the end of the array but before previously tested piece for this position
        std::swap(list.at(position - 1), list.at(list.size() - 1 - tries));
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
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    std::vector<PuzzlePiece> list;
    list.reserve(size * size);
    int nb = 1;
    for (Piece p : PIECES) {
        list.emplace_back(p, nb++);

    }
    std::vector<std::vector<int>> neighboursPosition(list.size());
    for (int i = 0; i < list.size(); ++i) {
        neighboursPosition.at(i) = (getAllAdjacent(i + 1));
    }

    solution(list, neighboursPosition);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    double temps = duration_cast<milliseconds>(t2 - t1).count();
    std::cout << temps << std::endl;

}
