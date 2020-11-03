///
/// @author   GUSTAVO CAMPOS
/// @author   GUSTAVO CAMPOS
/// @date   28/05/2019 19:44
/// @version  <#version#>
///
/// @copyright  (c) GUSTAVO CAMPOS, 2019
/// @copyright  Licence
///
/// @see    ReadMe.txt for references
///
//               GNU GENERAL PUBLIC LICENSE
//                Version 3, 29 June 2007
//
// Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
// Everyone is permitted to copy and distribute verbatim copies
// of this license document, but changing it is not allowed.
//
// Preamble
//
// The GNU General Public License is a free, copyleft license for
// software and other kinds of works.
//
// The licenses for most software and other practical works are designed
// to take away your freedom to share and change the works.  By contrast,
// the GNU General Public License is intended to guarantee your freedom to
// share and change all versions of a program--to make sure it remains free
// software for all its users.  We, the Free Software Foundation, use the
// GNU General Public License for most of our software; it applies also to
// any other work released this way by its authors.  You can apply it to
// your programs, too.
//
// See LICENSE file for the complete information

#include <assert.h>

#include <SPI.h>
#include <arduino.h>

#include "CorePartition.h"
#include "Terminal.hpp"
#include "epd4in2.h"
#include "epdpaint.h"

// Including External code
#include "CommandTerminal.hpp"
#include "E-PaperThread.hpp"

void setup ()
{
    Serial.begin (115200);

    // Max threads on system.
    if (CorePartition_Start (15) == false)
    {
        Serial.println ("Error staring up Threads.");
        exit (1);
    }

    assert (CorePartition_SetStackOverflowHandler (StackOverflowHandler));

    CorePartition_CreateThread (Thread_Serial, NULL, 256, 10);
}

/// Espcializing CorePartition Tick as Milleseconds
uint32_t CorePartition_GetCurrentTick ()
{
    return (uint32_t)millis ();
}

/// Specializing CorePartition Idle time
/// @param nSleepTime How log the sleep will lest
void CorePartition_SleepTicks (uint32_t nSleepTime)
{
    delay (nSleepTime);
}

/// Stack overflow Handler
void StackOverflowHandler ()
{
    while (!Serial)
        ;

    Serial.print (F ("[ERROR] - Stack Overflow - Thread #"));
    Serial.println (CorePartition_GetID ());
    Serial.println (F ("--------------------------------------"));
    // ShowRunningThreads ((Stream&)(Serial));
    Serial.flush ();
    exit (0);
}

void loop ()
{
    CorePartition_Join();
}