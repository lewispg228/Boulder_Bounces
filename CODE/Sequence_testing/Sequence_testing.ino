int gameBoard[] = {255, 255, 255, 255, 255, 255, 255, 255};
#define ROUNDS_TO_WIN 8
int RED_seq_count = 0;
int GREEN_seq_count = 0;
int BLUE_seq_count = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("hello");
  print_gameboard();
}

void loop()
{
  clear_gameBoard();
  create_new_gameBoard();
  delay(1000);
  print_gameboard();
}

void create_new_gameBoard(void)
{
  int newButton = random(0, 3);
  int on_deck_buttons[2] = {0, 0}; // these are used to help reandomize below

  ////////////  0

  gameBoard[0] = newButton;

  ////////////  1

  if (gameBoard[0] == 0)
  {
    on_deck_buttons[0] = 1;
    on_deck_buttons[1] = 2;
    gameBoard[1] = on_deck_buttons[random(0, 2)];
  }
  else if (gameBoard[0] == 1)
  {
    on_deck_buttons[0] = 0;
    on_deck_buttons[1] = 2;
    gameBoard[1] = on_deck_buttons[random(0, 2)];
  }
  else if (gameBoard[0] == 2)
  {
    on_deck_buttons[0] = 0;
    on_deck_buttons[1] = 1;
    gameBoard[1] = on_deck_buttons[random(0, 2)];
  }

  ////////////  2

  if (gameBoard[1] == 0)
  {
    if (gameBoard[0] == 1) gameBoard[2] = 2;
    else if (gameBoard[0] == 2) gameBoard[2] = 1;
  }
  else if (gameBoard[1] == 1)
  {
    if (gameBoard[0] == 0) gameBoard[2] = 2;
    else if (gameBoard[0] == 2) gameBoard[2] = 0;
  }
  else if (gameBoard[1] == 2)
  {
    if (gameBoard[0] == 0) gameBoard[2] = 1;
    else if (gameBoard[0] == 1) gameBoard[2] = 0;
  }

  ////////////  3

  gameBoard[3] = random(0, 3);

  ////////////  4

  update_button_counts();

  if (RED_seq_count >= 2)
  {
    on_deck_buttons[0] = 1;
    on_deck_buttons[1] = 2;
    gameBoard[4] = on_deck_buttons[random(0, 2)];
  }
  else if (GREEN_seq_count >= 2)
  {
    on_deck_buttons[0] = 0;
    on_deck_buttons[1] = 2;
    gameBoard[4] = on_deck_buttons[random(0, 2)];
  }
  else if (BLUE_seq_count >= 2)
  {
    on_deck_buttons[0] = 0;
    on_deck_buttons[1] = 1;
    gameBoard[4] = on_deck_buttons[random(0, 2)];
  }

  ////////////  5

  update_button_counts();

  if ((RED_seq_count >= 2) && (GREEN_seq_count >= 2)) gameBoard[5] = 2;
  else if ((RED_seq_count >= 2) && (BLUE_seq_count >= 2)) gameBoard[5] = 1;
  else if ((GREEN_seq_count >= 2) && (BLUE_seq_count >= 2)) gameBoard[5] = 0;
  else if (RED_seq_count >= 2)
  {
    on_deck_buttons[0] = 1;
    on_deck_buttons[1] = 2;
    gameBoard[5] = on_deck_buttons[random(0, 2)];
  }
  else if (GREEN_seq_count >= 2)
  {
    on_deck_buttons[0] = 0;
    on_deck_buttons[1] = 2;
    gameBoard[5] = on_deck_buttons[random(0, 2)];
  }
  else if (BLUE_seq_count >= 2)
  {
    on_deck_buttons[0] = 0;
    on_deck_buttons[1] = 1;
    gameBoard[5] = on_deck_buttons[random(0, 2)];
  }  

  ////////////  6

  update_button_counts();
  
  if (gameBoard[5] == 0)
  {
    on_deck_buttons[0] = 1;
    on_deck_buttons[1] = 2;
    gameBoard[6] = on_deck_buttons[random(0, 2)];
  }
  else if (gameBoard[5] == 1)
  {
    on_deck_buttons[0] = 0;
    on_deck_buttons[1] = 2;
    gameBoard[6] = on_deck_buttons[random(0, 2)];
  }
  else if (gameBoard[5] == 2)
  {
    on_deck_buttons[0] = 0;
    on_deck_buttons[1] = 1;
    gameBoard[6] = on_deck_buttons[random(0, 2)];
  }  

  ////////////  7

  update_button_counts();
  
  if (RED_seq_count >= 3)
  {
    on_deck_buttons[0] = 1;
    on_deck_buttons[1] = 2;
    gameBoard[7] = on_deck_buttons[random(0, 2)];
  }
  else if (GREEN_seq_count >= 3)
  {
    on_deck_buttons[0] = 0;
    on_deck_buttons[1] = 2;
    gameBoard[7] = on_deck_buttons[random(0, 2)];
  }
  else if (BLUE_seq_count >= 3)
  {
    on_deck_buttons[0] = 0;
    on_deck_buttons[1] = 1;
    gameBoard[7] = on_deck_buttons[random(0, 2)];
  }    


}

void print_gameboard(void)
{
  Serial.print("current gameboard: ");
  for (int i = 0; i < ROUNDS_TO_WIN ; i++)
  {
    Serial.print(gameBoard[i]);
    Serial.print(", ");
  }
  Serial.print("\tsequence counts(RGB): ");
  Serial.print(RED_seq_count);
  Serial.print(", ");
  Serial.print(GREEN_seq_count);
  Serial.print(", ");
  Serial.print(BLUE_seq_count);
  Serial.println(" ");
}

void clear_gameBoard(void)
{
  for (int i = 0 ; i < ROUNDS_TO_WIN ; i++)
  {
    gameBoard[i] = 255;
  }
}

void update_button_counts(void)
{
  RED_seq_count = 0;
  GREEN_seq_count = 0;
  BLUE_seq_count = 0;
  for (int i = 0 ; i < ROUNDS_TO_WIN ; i++)
  {
    if (gameBoard[i] == 0) RED_seq_count++;
    else if (gameBoard[i] == 1) GREEN_seq_count++;
    else if (gameBoard[i] == 2) BLUE_seq_count++;
  }
}
