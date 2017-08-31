# SimonGame
Repository for the Simon game project

This SimonGame project is about recreating the logic of Simon game. The Simon game is played by memorizing the flashing random sequential pattern shown on the device's lights and entering the memorized sequence through buttons. The game will start with a sequence of one and continue for multiple rounds with each consecutive rounds incrementing the pattern length by one, making the pattern longer to remember and the game more difficult as it goes on. The player loses when they enter the sequence incorrectly. The player wins when they enter the sequence correctly 9 times without error. When lose or win is determined, the project's LCD screen will display the number of rounds the player completed correctly and give them an option to play again with another random pattern.

NOTE: The .sm file was not used with RIMS to generate the .c file for this project. The .c file was hand-coded and the .sm file only gives a description of how the state machine of the game works.  

Components used:
•	4 LEDs
•	4 buttons
•	LCD
•	Wires
•	Micro-controller

USER GUIDE:
1. When the Simon Game is turned on, the LCD will display a "Welcome" message. Here, the player can press a button to begin the game.
2. When the game begins, the game's 4 LED lights will begin flashing and LCD will display the message "FLASHING". This indicates the game is outputting the sequence the player needs to memorize.
3. When the game finishes flashing its sequence, the LCD will display "WAITING". This indicates the game is waiting for the player to input their answer ONE BUTTON AT A TIME. If they input the correct sequence, the sequence length will be incremented by one unit and go back to "FLASHING".
NOTE: Each LED's respective button is of the same PIN number but is on a different port. Example: LED0 (which is on PIN0 of PORTB) has its respective button on PIN0 of PORTA.
NOTE: The game expects the player to be pressing one button at a time(press and release). Pressing multiple buttons simultaneously can result in an incorrect answer and cause the player to lose the game.
4. When the game detects an incorrectness in the player's input, the player loses and the game will display "YOU LOSE!" and output the number of rounds done correctly. If 9 rounds are done correctly, the player wins and the game will display "YOU WIN!" and output 9 rounds done correctly. 
5. When the player lost or won, the player can press any button to go back to its START Menu and play again.


