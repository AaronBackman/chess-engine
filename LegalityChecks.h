#ifndef LEGALITY_CHECKS_H
#define LEGALITY_CHECKS_H

u64 getThreatMap(int originSquare, int side);
bool isSquareThreatened(int originSquare, int side);
bool isKingThreatened();
bool checkIfNoLegalMoves(int side);

#endif