// Berserk is a UCI compliant chess engine written in C
// Copyright (C) 2022 Jay Honnold

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef ENDGAME_H
#define ENDGAME_H

#include "types.h"
#include "util.h"

#define WINNING_ENDGAME 20000

int MaterialValue(Board* board, const int side);
int EvaluateKPK(Board* board, const int winningSide);
int EvaluateKXK(Board* board, const int winningSide);
int EvaluateKBNK(Board* board, const int winningSide);
int EvaluateKnownPositions(Board* board);

uint8_t GetKPKBit(uint32_t bit);
uint32_t KPKIndex(int winningKing, int losingKing, int pawn, int stm);
uint8_t KPKDraw(int winningSide, int winningKing, int losingKing, int pawn, int stm);

#endif