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

#define WINNING_ENDGAME 20000

#define WINNING_KXK_PAWN_ENDGAME_OFFSET 0
#define WINNING_KXK_PAWN_QUEENING_OFFSET 10
#define WINNING_KXK_PAWN_KING_OFFSET 2

#define WINNING_KXK_MAJOR_ENDGAME_OFFSET \
  (WINNING_KXK_PAWN_ENDGAME_OFFSET + WINNING_KXK_PAWN_QUEENING_OFFSET * 10 + WINNING_KXK_PAWN_KING_OFFSET * 10)

#define WINNING_ENDGAME_LOSING_KING_RANK_OFFSET 2
#define WINNING_ENDGAME_LOSING_KING_FILE_OFFSET WINNING_ENDGAME_LOSING_KING_RANK_OFFSET
#define WINNING_ENDGAME_WINNING_KING_DIS_OFFSET 10

int EvaluateKXK(Board* board);

uint8_t GetKPKBit(uint32_t bit);
uint32_t KPKIndex(int ssKing, int wsKing, int p, int stm);
uint8_t KPKDraw(int ss, int ssKing, int wsKing, int p, int stm);

#endif