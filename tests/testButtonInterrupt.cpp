/*
 * isr.c:
 *	Wait for Interrupt test program - ISR method
 *
 *	How to test:
 *	  Use the SoC's pull-up and pull down resistors that are avalable
 *	on input pins. So compile & run this program (via sudo), then
 *	in another terminal:
 *		gpio mode 0 up
 *		gpio mode 0 down
 *	at which point it should trigger an interrupt. Toggle the pin
 *	up/down to generate more interrupts to test.
 *
 * Copyright (c) 2013 Gordon Henderson.
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <csignal>

// What GPIO input are we using?
//	This is a wiringPi pin number

#define	BUTTON_PIN	0

// globalCounter:
//	Global variable to count interrupts
//	Should be declared volatile to make sure the compiler doesn't cache it.

static volatile int globalCounter = 0 ;
static volatile bool displayState = false;

void fallingEdge(void)
{
    ++globalCounter ;
    int pinState = digitalRead(BUTTON_PIN);
    if(!pinState) {
        displayState = !displayState;
        printf("fallingEdge, gc=%d, state=%d\n", globalCounter, displayState);
    }
}

void quit(int signal)
{
    printf("Caught SIGINT, exiting...\n");
    exit(0);
}

/*
 *********************************************************************************
 * main
 *********************************************************************************
 */

int main (void)
{
    int myCounter = 0 ;

    if (wiringPiSetup () < 0)
    {
        fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno)) ;
        return 1 ;
    }

    // Set the button pin to input mode
    pinMode(BUTTON_PIN, INPUT);
    // Set the pull up resistor to up to fix it at 3.3V so it reads high until button is pressed
    pullUpDnControl(BUTTON_PIN, PUD_UP);

    // Register the interrupt callback function
    if (wiringPiISR (BUTTON_PIN, INT_EDGE_FALLING, &fallingEdge) < 0) {
        fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno)) ;
        return 1 ;
    }
    std::signal(SIGINT, quit);

    for (;;)
    {
        printf ("Waiting ... ") ; fflush (stdout) ;

        while (myCounter == globalCounter)
            delay (100) ;

        printf (" Done. counter: %5d\n", globalCounter) ;
        myCounter = globalCounter ;
    }

    return 0 ;
}