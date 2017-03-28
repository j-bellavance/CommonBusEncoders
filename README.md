# CommonBusEncoders
Encoders are linked to 3 common busses to save pins

I had a project that needed 19 encoders, and, using three pins per encoder was not a good solution for me.

I saw that it was possible to add that many encoders with only 22 pins. Thanks to Vlad Sychev, the designer of ArdSim for his design.

The ldea is to link all the A pins to a bus, the B pins to a second bus and one of the switch pins to a third bus. 
Only the common pin needs it's own arduino digital pin.

This library can handle as many encoders as you need. 

This Library is to be used with encoders that have detents. (2 or 4 steps per detent can be used). 
It will return a value only when the encoder reaches a click. 
It is best used to change discrete values (integers, letters of the alphabet, angle of rotation in degres, selecting an item in a menu...).

The Library is to be used with encoders that are turned by hand. It will not be fast enough to read motor mounted encoders. It does not rely on interrupts, and may miss the first click... and subsequent ones. Other Libraries handle fast turning encoders in a very efficient manner, one encoder at the time.

It is good for applications where the user has a feedback of the rotation of an encoder. This way, missing a click is not critical, as the user can react and correct.

In every loop, all the encoders are read to detect movement. When such a movement is detected, the Library will focus on this encoder, as long as it is beeing operated. When there is no more activity (for a time period that can be adjusted in the script) the Library will read all the encoders again in every loop.
To accomodate mechanical encoders, a debounce layer has been added. It's sensitivity can be adjusted between 5us to 160us (settings 1 to 32).

With all this said, I have a sketch that reads 19 encoders and 35 switches, (and does something with the reads) and it is rock solid.
