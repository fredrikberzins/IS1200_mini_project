# IS1200_mini_project
## Project Description
Develop a version of pong for the chipKIT Uno32 board with a chipKIT Basic I/O shield as well as an optional control system.

## Goal
- The game must be rendered on a display.
- The ball must move in all directions and bounce naturally.
- The paddles must move in the y-direction.
- The game must detect if a player wins and display the results.

## Execution guide (PIC32MX (300/400 series) and MSYS)
- Run 'make' in the current directory.
- Run 'make install TTYDEV=/dev/ttyX' where X depends on your serial port.
    - Find the currently used serial port (X) with 'ls /dev' 