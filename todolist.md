# Game Logic todo
1. Button code under "uartButton" needs to be converted to GPIO programming. A thing to note the logic currently tracks if the button is released and not when pressed so the new version needs to follow that same logic. This was done to not repeat inputs and make gameplay work much better. Similarily a delay() needs to be made in GPIO but I can see if millis() can be used the same way since its allowed.
2. Finish the last bit of gameLogic, it will use the buttons with the Arduino library(delay, pinMode) and then converted later to GPIO.
3. Debugging on game
4. Import render code into gameLogic, code is BASICALLY done just needs some extra logic is all
5. Integrate back into project, shouldn't be too hard as the 1 class I created for this is(and the LCD files) already in c++ and the logic should transfer fine. Howevever, the LCD and GPIO programming might not act nice so we will see.
6. Another note is if using the Arduino IDE the header files need to be put into another folder somewhere in appdata on your machine or its equivalent.