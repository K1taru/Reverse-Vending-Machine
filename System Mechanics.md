Reverse Vending Machine - Where recycled plastic or metals can be exchanged to for water

First, the user will manually detect their object on the sensor, those sensor are located above the shoot.
There is a flipping board connected with two servo motors (left facing and right facing) that seals the shoot to prevent other materials going inside without validation.

The system only accepts metals or plastics, if a biodegradable material is detected, the shoot board will not open and will prompt to the user that biodegradable are not accepted. Else, the shoot will open.

There are two trash bins inside of the machine, plastic is left and right for metals. In between there is a ply board, and on top of the ply board is another flapper. 

The flapper is a board attached with two servo motors (left facing and right facing). The board is positioned 90 degrees which is aligned to the ply board divider.

The moment a valid meterial was detected (plastic or metal), the flapper board should move -45 degrees or 45 degrees (-45 degrees if metal, material will be guided and bounced to the right bin. otherwise move 45 degrees to guide the material to left bin). There is a vibration sensor attached to the flapper board, so the moment it detected a vibration, the system automatically will detect an object was really put inside. Then the position of the flapper board will return immediately to its original position, then the shoot board will close too.

Every material validated is equivalent to 1 point, and to avail a water, it needs 3 points. If ever a user just put 2 objects, do not reset the count, even if the user does not take the water, do not reset the count. This is to ensure that if the user want to donate his water, the next one can take it. If the user has not enough bottles, then at least the next person jjust needs two or one bottle to shoot to gain a water.

There will be button beside the dispenser. if the system has water credits available, when the button is pushed then it will dispense water for a given set of time.

DEV:
- Create global variables so that they are easier to modify
- Delay time, duration, count limits, and other variables that are subjected to be changed or costomized shouldd be also on the global variables.
- The LCD is 20x4 so make sure the lcd shows information the user should see
- Make the pin #define assignments near the global variables or on top of them so that they are easily modified.
