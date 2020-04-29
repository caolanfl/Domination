#include <stdio.h>
#include <stdlib.h>
#include "board.h"

int main() {
  int turn = 0; // Holds turn number. 0 for player 1 and 1 for player 2

  // Generates the 10*10 game board
  // Initializes the "board" variable a 10*10 array of Piece structs 
  // More info on it is in the "board.h" header file
  generate_board();

  while(1) {
    // Output whose turn it is then output the board
    if(turn) {
      printf(GREEN "\n Player %d's move : \n\n" COLOR_RESET, turn+1);
    } else {
      printf(RED "\n Player %d's move : \n\n" COLOR_RESET, turn+1);
    }
    output_board();

    // Checks if player wants to use extra pieces available to them
    int use_extra_piece = prompt_reserved_move(turn);

    int valid_move = 0;
    while(!valid_move) {
      // Take the initial and destination positions of stack player wants to move
      int init_pos[2], dest_pos[2];
      // Don't take initial position as input if the player decides to place one of their extra pieces
      if(!use_extra_piece) {
        printf("\n Move from : ");
        scanf("%d %d", &init_pos[0], &init_pos[1]);
      }
      printf("\n Move to : ");
      scanf("%d %d", &dest_pos[0], &dest_pos[1]);

      // Make move and keep track of extra pieces cut off at end of stack
      valid_move = move_stack(init_pos, dest_pos, turn, use_extra_piece);
    }

    // Win condition
    if(winning_position()) {
      if(turn) { printf("Player 2 won."); } else { printf("Player 1 won."); }
    }

    // Invert turn after loop
    turn = !turn; 
  }

  return 0;
}
