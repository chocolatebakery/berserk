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

const uint8_t kpkResults[2 * 64 * 64 * 24 / 8] = {
#include "kpk.h"
};

// Push a king into a corner
int EvaluatePushOffset(Board* board, int winningSide) {
  int offset = 0;
  int losingSide = winningSide ^ 1;

  int winningKing = lsb(PieceBB(KING, winningSide));
  int losingKing = lsb(PieceBB(KING, losingSide));
  int losingKingRankDis = Rank(losingKing) ^ (7 * (Rank(losingKing) > 3));
  int losingKingFileDis = File(losingKing) ^ (7 * (File(losingKing) > 3));

  offset += WINNING_ENDGAME_WINNING_KING_DIS_OFFSET * (7 - Distance(winningKing, losingKing)) +
            WINNING_ENDGAME_LOSING_KING_FILE_OFFSET * (3 - losingKingFileDis) +
            WINNING_ENDGAME_LOSING_KING_RANK_OFFSET * (3 - losingKingRankDis);

  return offset;
}

int EvaluateKXK(Board* board) {
  if (PieceBB(QUEEN, WHITE) | PieceBB(QUEEN, BLACK) | PieceBB(ROOK, WHITE) | PieceBB(ROOK, BLACK)) {
    int winningSide = (PieceBB(QUEEN, WHITE) | PieceBB(ROOK, WHITE)) ? WHITE : BLACK;
    int eval = WINNING_ENDGAME + WINNING_KXK_MAJOR_ENDGAME_OFFSET + EvaluatePushOffset(board, winningSide);

    return winningSide == board->stm ? eval : -eval;
  } else if (PieceBB(PAWN, WHITE) | PieceBB(PAWN, BLACK)) {
    int eval = WINNING_ENDGAME + WINNING_KXK_PAWN_ENDGAME_OFFSET;
    int winningSide = PieceBB(PAWN, WHITE) ? WHITE : BLACK;
    int losingSide = winningSide ^ 1;

    int winningSideKing = lsb(PieceBB(KING, winningSide));
    int losingSideKing = lsb(PieceBB(KING, losingSide));

    int pawn = lsb(PieceBB(PAWN, winningSide));

    if (KPKDraw(winningSide, winningSideKing, losingSideKing, pawn, board->stm)) return 0;

    // Get the queening square, if black is winning shift it to > 56
    int queeningSquare = File(pawn) + A1 * winningSide;
    eval += WINNING_KXK_PAWN_QUEENING_OFFSET * (7 - Distance(pawn, queeningSquare)) +
            WINNING_KXK_PAWN_KING_OFFSET * (7 - Distance(winningSideKing, queeningSquare));

    return winningSide == board->stm ? eval : -eval;
  }

  return UNKNOWN;
}

// The following KPK code is modified for my use from Cheng (as is the dataset)
uint8_t GetKPKBit(uint32_t bit) { return (uint8_t)(kpkResults[bit >> 3] & (1U << (bit & 7))); }

uint32_t KPKIndex(int ssKing, int wsKing, int p, int stm) {
  int file = File(p);
  int x = file > 3 ? 7 : 0;

  ssKing ^= x;
  wsKing ^= x;
  p ^= x;
  file ^= x;

  uint32_t pawn = (((p & 0x38) - 8) >> 1) | file;

  return (uint32_t)ssKing | ((uint32_t)wsKing << 6) | ((uint32_t)stm << 12) | ((uint32_t)pawn << 13);
}

uint8_t KPKDraw(int ss, int ssKing, int wsKing, int p, int stm) {
  uint32_t x = (ss == WHITE) ? 0u : 0x38u;
  uint32_t idx = KPKIndex(ssKing ^ x, wsKing ^ x, p ^ x, ss ^ stm);

  return GetKPKBit(idx);
}