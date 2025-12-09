# Wetris Game Logic
I was debating between 2 main ideas specifically for how to make the pieces.
1. Use only 1 special character of a single block then glue multiple of them together to get the pieces.
2. Give each shape its own character

Obviously this probably depends if wether we are struggling on special characters but the modularity of using blocks could be nice. Also if we wanted to add visual flourish to the pieces the blocks would make it easy. In terms of the level issue I don't know if I missed any discussion on it but we could:
1. At certain point thresholds the speed increases which I think counts as multiple levels
2. Or we go with the path Gabe noted in the brainstorming and use a game over as a "level"
The ISR will probably be used for block updates as Gabe mentioned but to keep how tetris works normally player input will be seperate.

So in my head the current structure:
Use a 2D array to make it easier to track where blocks are and drawing it to the LCD if this eats too much memory we can try something else. Then collect user-input and update positioning during interrupt. Check for Tetris and if player fails check for Wetris. The check will be a simple loop through the 2D array that hopefully wont be too memory intensive. Other details I am considering is moving the score from the LCD to the terminal and have a "next piece" displayed on the LCD instead but it is hopefully possible to have both. Since there may be levels game will run off a variable tick rate. The collision check will check if the spaces under and around the piece in play and again hope that memory isn't terrible. I'll probably make a seperate .ino file to test this all out then later integrate it back into the rest of project and hope that doesn't end too poorly.

Note on the LCD pins: follow the hardware_SPI pin column when applicable