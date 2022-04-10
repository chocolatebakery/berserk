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

#include "endgame.h"

#include <assert.h>
#include <stdlib.h>

#include "bits.h"
#include "board.h"
#include "search.h"
#include "see.h"

const uint8_t kpkResults[2 * 64 * 64 * 24 / 8] = {
#include "kpk.h"
};

inline int MaterialValue(Board* board, const int side) {
  int staticScore = 0;
  for (int piece = PAWN; piece <= QUEEN; piece++) staticScore += bits(PieceBB(piece, side)) * SEE_VALUE[piece];

  return staticScore;
}

inline int EvaluateKPK(Board* board, const int winningSide) {
  const int losingSide = winningSide ^ 1;
  int eval = WINNING_ENDGAME;

  int winningKing = lsb(PieceBB(KING, winningSide));
  int losingKing = lsb(PieceBB(KING, losingSide));

  int pawn = lsb(PieceBB(PAWN, winningSide));

  if (KPKDraw(winningSide, winningKing, losingKing, pawn, board->stm)) return 0;

  int queeningSquare = File(pawn) + A1 * winningSide;
  eval += 5 * (7 - Distance(pawn, queeningSquare)) + (7 - Distance(winningKing, queeningSquare));

  return winningSide == board->stm ? eval : -eval;
}

inline int EvaluateKXK(Board* board, const int winningSide) {
  const int losingSide = winningSide ^ 1;
  int eval = WINNING_ENDGAME + MaterialValue(board, winningSide);

  int winningKing = lsb(PieceBB(KING, winningSide));
  int losingKing = lsb(PieceBB(KING, losingSide));
  int losingKingRankDist = min(Rank(losingKing), 7 - Rank(losingKing));
  int losingKingFileDist = min(File(losingKing), 7 - File(losingKing));

  eval += 10 * (7 - Distance(winningKing, losingKing)) + 6 - losingKingFileDist - losingKingRankDist;

  return winningSide == board->stm ? eval : -eval;
}

int EvaluateKnownPositions(Board* board) {
  switch (board->piecesCounts) {
    // See IsMaterialDraw
    case 0x0:       // Kk
    case 0x100:     // KNk
    case 0x200:     // KNNk
    case 0x1000:    // Kkn
    case 0x2000:    // Kknn
    case 0x1100:    // KNkn
    case 0x10000:   // KBk
    case 0x100000:  // Kkb
    case 0x11000:   // KBkn
    case 0x100100:  // KNkb
    case 0x110000:  // KBkb
      return 0;
    case 0x1:  // KPk
      return EvaluateKPK(board, WHITE);
    case 0x10:  // Kpk
      return EvaluateKPK(board, BLACK);
    case 0x1000000:    // KRk
    case 0x100000000:  // KQk
      return EvaluateKXK(board, WHITE);
    case 0x10000000:    // Krk
    case 0x1000000000:  // Kqk
      return EvaluateKXK(board, BLACK);
    default:
      break;
  }

  return UNKNOWN;
}

// The following KPK code is modified for my use from Cheng (as is the dataset)
uint8_t GetKPKBit(uint32_t bit) { return (uint8_t)(kpkResults[bit >> 3] & (1U << (bit & 7))); }

uint32_t KPKIndex(int winningKing, int losingKing, int pawn, int stm) {
  int file = File(pawn);
  int x = file > 3 ? 7 : 0;

  winningKing ^= x;
  losingKing ^= x;
  pawn ^= x;
  file ^= x;

  uint32_t p = (((pawn & 0x38) - 8) >> 1) | file;

  return (uint32_t)winningKing | ((uint32_t)losingKing << 6) | ((uint32_t)stm << 12) | ((uint32_t)p << 13);
}

uint8_t KPKDraw(int winningSide, int winningKing, int losingKing, int pawn, int stm) {
  uint32_t x = (winningSide == WHITE) ? 0u : 0x38u;
  uint32_t idx = KPKIndex(winningKing ^ x, losingKing ^ x, pawn ^ x, winningSide ^ stm);

  return GetKPKBit(idx);
}