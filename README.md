ProjectHaloUK
=============

A place for all Project Halo related materials
==============================================

History
-------
Project Halo started as a concept to integrate BMW Angel eye style DRLs into the 159/Brera/Spider triple headlight configuration. Initial development work was completed by myself & YonasH. YonasH pioneered the first sets of Halo's in a kit form that could be bought and installed as a DIY solution, however both of our efforts to disassemble the headlights resulted in failure or breakage and the only viable solution was to cut the lenses off, install the rings and glue the lenses back on.

As this was not a viable option, development ceased for around a year before starting up again after discussions with Coxy1 on headlight disassembly. Coxy1 was able to pioneer an approach the taking the headlights apart, but only if they were new ones where the glue had not yet had time to cure fully (several years of use on a car cures them fully!!)

Once this had been solved, I restarted development on the electronic controllers for the headlights to meet UK specifications and simplify the amount of electronics & wiring needed in comparison to the approach YonasH took.


Product
-------
This is what the headlights look like on the car:

[YOUTUBE]http://www.youtube.com/watch?v=qOf9f99d9EU[/YOUTUBE]

Opperation
---------
This is a description of there operation:

On start-up, they run Mode 1 once, then default to Mode 2 below:

Mode 1:           Fade from 0% to 100% to 90% in sequence from Led 1 (inside) to Led 3 (outside)
Mode 2:           Daytime operation | all Leds at 90%
Mode 3:           Night operation | all Leds at 80%
Mode 4:           Indicator Mode | Outer Led & inner Led rings at 50% - middle ring off
Mode 5:           Main Beam | All rings at full power

Modes 3,4 & 5 are selected by +12v signal on each of the 3 inputs to the controllers. There is a 400ms delay after the indicator stops before the rings return to Mode 2,3 or 5. This is to allow the controllers to detect that the indicator has stopped flashing. I nay yet get that down to 250-300ms but any less and it doesn't work properly.

FAQ
---

Q) Is this available as a DIY kit to install in my existing headlights?
 - Yes

Q) Will the units work with my factory fit Xenons?
- Yes they will

Q) Will these units work with after market HID kits, LEDs and other lighting mods?
- Yes they will.

Q) Will these units work with my automatic headlight feature?
- Yes, the switching between modes of operating is automatically controlled by the electronics inside the new headlights so it doesn't matter how they switch on and off.

Q) How long will they last?
- The LED rings are quoted as having 20000 hours lifespan, but I can't vouch for that!! What I can say is we have done everything electrically viable to guarantee the LED's the most optimum / safe / stable conditions fully isolated and protected from the car's electronics system so they will last as long as they possibly could.

Q) will they work with other vehicles?
- Technically the controller units can run 1, 2 or 3 rings each, so they could easily be adapted to be used in 147, 156, & GT's. We would just need to modify the code, and measure the headlights to get the right rings & set power loads.
