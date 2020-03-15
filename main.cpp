/* ---------------------------
Laboratoire :
Fichier : main.cpp
Auteur(s) : Besseau Zwick Viotti
Date : 16-03-2020

But : Le but de ce laboratoire consiste à trouver toutes les solutions au jeu décrit sur la page suivante: https://asd1-heigvd.github.io/ASD1-Labs/puzzle/

Remarque(s) :

Compilateur : gcc version 7.4.0

--------------------------- */
#include <iostream>
#include "pieces.h"

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
     * @param the side on which to put the piece (Only on the left or Top side)
     * @return true if the two piece can be neighbour, false otherwise
     */
    bool canBeNeighbour(const PuzzlePiece &piece, Side side) const;

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

bool PuzzlePiece::canBeNeighbour(const PuzzlePiece &piece, Side side) const {
    Side ownSide;
    Side pieceSide;
    if (side == Side::LEFT) {
        ownSide = Side::LEFT;
        pieceSide = Side::RIGHT;
    } else { // UP
        ownSide = Side::TOP;
        pieceSide = Side::DOWN;
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
 * Get a list of the possible orientation for a given piece depending of the piece already present on the board
 * @param list a vector of puzzlePiece containing the puzzle
 * @param piece the piece to test
 * @return a vector of orientation. Is empty if the piece doesn't match at all
 */
std::vector<char>
getValidOrientation(std::vector<PuzzlePiece> &list, PuzzlePiece &piece) {
    std::vector<char> output;
    int position = piece.getPosition();
    --position;

    int posLeft = position % size > 0 ? position - 1 : -1;
    int posTop = position >= size ? position - size : -1;

    for (char orientation = 'a'; orientation <= 'd'; ++orientation) {
        piece.setOrientation(orientation);
        if (posLeft != -1) {
            if (!piece.canBeNeighbour(list.at(posLeft), Side::LEFT)) {
                continue;
            }

        }
        if (posTop != -1) {
            if (!piece.canBeNeighbour(list.at(posTop), Side::TOP)) {
                continue;
            }
        }
        output.push_back(orientation);
    }
    return output;
}

/**
 * Solve and display all the solution of the puzzle
 * @param list a vector of puzzlePiece containing the puzzle
 * @param position the position of the puzzle to test (default is one for the starting position)
 * @return a boolean (only used in the recursion to indicate if a path is impossible)
 */
bool
solution(std::vector<PuzzlePiece> &list, int position = 1) {
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
        // Set the pointer to the correct element
        current = &list.at(position - 1);

        std::vector<char> validOrientation = getValidOrientation(list, *current);

        for (char orientation : validOrientation) {
            current->setOrientation(orientation);
            // If last piece display solution
            if (position == list.size()) {
                for (const PuzzlePiece p : list) {
                    std::cout << p << " ";
                }
                std::cout << std::endl;
            } else {
                solution(list, position + 1);
            }
        }

        current->setPosition(-1);
        // Put piece tested at the end of the array but before previously tested piece for this position
        std::swap(list.at(position - 1), list.at(list.size() - 1 - tries));
        ++tries;

        possiblePiece.pop_back();

        if (possiblePiece.empty()) {
            return false;
        }
        current = possiblePiece.back();
    }
}

int main() {
    std::vector<PuzzlePiece> list;
    list.reserve(size * size);
    for (int i = 0; i < PIECES.size(); ++i) {
        list.emplace_back(PIECES.at(i), i + 1);

    }

    solution(list);

}
