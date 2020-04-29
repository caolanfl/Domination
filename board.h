#include <stdio.h>
#include <stdlib.h>

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGNETA "\x1b[35m"
#define CYAN "\x1b[36m"
#define COLOR_RESET "\x1b[0m"

// A struct that represents a node in a linked list.
// Team is 0 for a piece that is player 1's and 1 for player 2's
// Next points to the next piece in the stack and is "NULL" for the last piece in a stack
struct Piece {
  int team;
  struct Piece* next;
};
typedef struct Piece piece_t;

// This data structure represents the 10*10 board
// Each piece on the board is of type piece_t 
piece_t board[8][8];
int extra_pieces[2] = {0,0}; // Keeps track of extra pieces each player has available. First value is for player 1, second for player 2.
int captured_pieces[2] = {0,0}; // Keeps track of the number of pieces each player has captured. First value is for player 1, second for player 2.

// Takes board cords as input and returns 1 if the cords are in a corner
int is_corner(int x, int y) {
  if(x+y < 2) { return 1; }                 // Top left corner
  if(abs(x-7)+y < 2 && x>=6) { return 1; }   // Top right corner
  if(x+abs(y-7) < 2 && y>=6) { return 1; }   // Bottom left corner
  if(x+y > 12) { return 1; }                // Bottom Right corner
  return 0;                                 // Default to not corner
}

// This function is called once to fill the array board with the starting game positions
void generate_board(){
  for(int y=0; y<7; ++y) {
    for(int x=0; x<7; ++x) {
      if(x>0 && x<7 && y>0 && y<7) {
        // Memory is reserved for a piece
        piece_t *piece = malloc(sizeof(piece_t));
        // The team of the piece is set
        // 
        piece->team = (int)(((x-1)+(6*y))/2) % 2 == 0;
        piece->next = NULL;
        piece_t *stack_head = malloc(sizeof(piece_t));
        stack_head->team = 3;
        stack_head->next = piece;
        board[y][x] = *stack_head;
      } else {
        piece_t *stack_head = malloc(sizeof(piece_t));
        stack_head->team = 3;
        stack_head->next = NULL;
        board[y][x] = *stack_head;
      }
    }
  }
}

piece_t * get_bottom_piece(int y, int x) {
  piece_t *temporary = &board[y][x];
  piece_t *bottom_init_stack_p;
  while(temporary != NULL) {
    bottom_init_stack_p = temporary;
    temporary = temporary->next;
  }
  return bottom_init_stack_p;
}

// Returns 1 if move is vaild and was made
int move_stack(int init_pos[2], int dest_pos[2], int turn, int use_extra_piece) {
  // Following code deals with checking if a move is valid or not
  // Move isn't valid if player tries to move anything from or to corners
  if(is_corner(init_pos[0], init_pos[1]) || is_corner(dest_pos[0], dest_pos[1])) { 
    printf("\n Invalid move. Not a valid board position. \n");
    return 0; 
  }                         
  // Move isn't valid if player tries to move stack at empty coords
  if(board[init_pos[0]][init_pos[1]].next == NULL) { 
    printf("\n Invalid move. You can't move an empty stack. \n");
    return 0; 
  }
  // Move isn't valid if player tries to move stack with opposing teams piece on top
  if(board[init_pos[0]][init_pos[1]].next->team != turn) { 
    printf("\n Invalid move. You can't move your opponent's piece. \n");
    return 0; 
  }
  // Move isn't valid if player tries to use coords not on the board
  if(init_pos[0]<0 || init_pos[0]>7 || init_pos[1]<0 || init_pos[1]>7 || dest_pos[0]<0 || dest_pos[0]>7 || dest_pos[1]<0 || dest_pos[1]>7) {
    printf("\n Invalid move. Coords off of board. \n");
    return 0; 
  }

  // Code that deals with moving piece
  piece_t *bottom_init_stack_p;
  if(use_extra_piece) {
    bottom_init_stack_p = malloc(sizeof(piece_t));
    bottom_init_stack_p->team = turn;
    bottom_init_stack_p->next = NULL;
  } else {
    bottom_init_stack_p = get_bottom_piece(init_pos[0], init_pos[1]);
  }

  bottom_init_stack_p->next = board[dest_pos[0]][dest_pos[1]].next;
  if(!use_extra_piece) {
    board[dest_pos[0]][dest_pos[1]].next = board[init_pos[0]][init_pos[1]].next;
    board[init_pos[0]][init_pos[1]].next = NULL;
  } else {
    board[dest_pos[0]][dest_pos[1]].next = bottom_init_stack_p;
  }

  // Count number of current players pieces over 5 limit in stack and shorten stack
  int i = 0;
  
  piece_t *temporary = &board[dest_pos[0]][dest_pos[1]];
  while(temporary != NULL) {
    int delete_piece = 0;
    if(i > 5) {
      if(temporary->team == turn) {
        ++extra_pieces[turn];
      } else {
        ++captured_pieces[turn];
      }
    }
    if(i >= 5) {
      delete_piece = 1;
    }
    ++i;

    piece_t *prev_temp = temporary;
    temporary = temporary->next;
    if(delete_piece) {
      prev_temp->next = NULL;
      prev_temp = NULL;
      free(prev_temp);
    }
  }
  return 1;
}

int prompt_reserved_move(int turn) {
  int use_extra_piece = 0;
  // Runs if player has extra pieces available
    if(extra_pieces[turn]) {
      // Prompt user on whether they want to use an extra piece
      printf("\n You have %d available extra pieces.", extra_pieces[turn]);
      printf("\n Would you like to place one? Y/N : ");
      char choice;
      scanf(" %c", &choice);
      // If the player submits "Y" then use_extra_piece is true
      use_extra_piece = (choice=='Y');
      --extra_pieces[turn];
    }
    return use_extra_piece;
}

void output_board() {
  for(int yy=0; yy<9; ++yy) {
    for(int xx=0; xx<9; ++xx) {
      if(yy == 0) {
        if(xx != 0) {
          printf(CYAN "%d     " COLOR_RESET, xx-1);
        } else {
          printf("      ");
        }
        continue;
      }
      if(xx == 0) {
        printf(CYAN "%d     " COLOR_RESET, yy-1);
        continue;
      }

      int y = yy-1;
      int x = xx-1;

      piece_t *temporary = board[y][x].next;
      int i = 6;

      // Print nothing for the corners
      if(!is_corner(y, x)) { 
        if(temporary == NULL) { printf("-"); --i; }
        while(temporary != NULL) {
          i -= 1;
          if(temporary->team == 0) {
            printf(RED "O" COLOR_RESET);
          } else {
            printf(GREEN "O" COLOR_RESET);
          }
          temporary = temporary->next;
        }
      }
      while(i) {
        printf(" ");
        --i;
      }
    }
    printf("\n\n");
  }
}

int winning_position() {
  int p1_count = 0;
  int p2_count = 0;

  for(int y=1; y<7; ++y) {
    for(int x=1; x<7; ++x) {
      piece_t *stack = board[y][x].next;
      if(stack == NULL) { continue; }
      if(stack->team == 0) { ++p1_count; }
      if(stack->team == 1) { ++p2_count; }
    }
  }

  return (p1_count == 0) || (p2_count == 0);
}