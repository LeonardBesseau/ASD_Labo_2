#include <iostream>
#include "pieces.h"

bool isPossible(AttachementType a, AttachementType b) {
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


bool checkAdjacent(Piece a, Piece b, char orientation) {
    // UP RIGHT DOWN LEFT
    switch (orientation){
        case 'a':
            return isPossible(a.at(0), b.at(2));
        case 'b':
            return isPossible(a.at(1), b.at(3));
        case 'c':
            return isPossible(a.at(2), b.at(0));
        default:
            return  isPossible(a.at(3), b.at(1));
    }
}

int displayPossibleRotation(Piece a, Piece b) {
    int nb = 0;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (isPossible(a.at(i), b.at(j))) {
                ++nb;
                std::cout << "Possible " << i << ":" << j << std::endl;
            }
        }
    }
    return nb;
}

int main() {
    int nb = 0;
    std::vector<int> a(9);
    for (int i = 0; i < 9; ++i) {
        int occ = 0;
        for (int j = 0; j < 9; ++j) {
            if(i == j){
                continue;
            }
            nb++;
            std::cout << "Piece "<<i<<" with " <<j<<std::endl;
            occ +=displayPossibleRotation(PIECES.at(i), PIECES.at(j));
        }
        a.at(i) = occ;
    }
    std::cout << "Nombre "<<nb<<std::endl;
    for(int c : a){
        std::cout <<c<<std::endl;
    }
}
