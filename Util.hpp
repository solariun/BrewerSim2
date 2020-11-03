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

#ifndef UTIL_HPP
#define UTIL_HPP 

#include "CorePartition.h"

#include "Arduino.h"

#include <LedControl.h>
#include <Wire.h>
#include <assert.h>
#include <string>


//Topics and definitions 
const char* topicDisplay = "display";
#define MATRIX_DISPLAY_CHANGE 1


//Standard functions 
void SetLocation (Stream& device, uint16_t nY, uint16_t nX)
{
    uint8_t szTemp[15];
    uint8_t nLen = snprintf ((char*)szTemp, sizeof (szTemp), "\033[%u;%uH", nY, nX);

    device.write (szTemp, nLen);
}

// workis with 256 colors
void SetColor (Stream& device, const uint8_t nFgColor, const uint8_t nBgColor)
{
    byte szTemp[15];
    uint8_t nLen = snprintf ((char*)szTemp, sizeof (szTemp), "\033[%u;%um", nFgColor + 30, nBgColor + 40);

    device.write (szTemp, nLen);
}

void LocalEcho (Stream& device, bool state)
{
    device.flush ();
    device.print ("\033[12");
    device.print (state ? "l" : "h");
    device.flush ();
}

void ResetColor (Stream& device)
{
    device.print ("\033[0m");
}

void HideCursor (Stream& device)
{
    device.print ("\033[?25l");
}

void ShowCursor (Stream& device)
{
    device.print ("\033[?25h");
}

void ClearConsole (Stream& device)
{
    device.print ("\033c\033[2J");
}

void ReverseColor (Stream& device)
{
    device.print ("\033[7m");
}

void Delay (uint64_t nSleep)
{
    uint32_t nMomentum = millis ();

    // delay (nSleep); return;

    do
    {
        CorePartition_Yield ();
    } while ((millis () - nMomentum) < nSleep);
}

void ShowRunningThreads (Stream& client)
{
    size_t nCount = 0;

    client.println ();
    client.println (F ("Listing all running threads"));
    client.println (F ("--------------------------------------"));
    client.println (F ("ID\tName\tStatus\tNice\tStkUsed\tStkMax\tCtx\tUsedMem\tExecTime"));

    for (nCount = 0; nCount < CorePartition_GetNumberOfActiveThreads (); nCount++)
    {
        if (CorePartition_GetStatusByID (nCount) > 0)
        {
            client.print (F ("\e[K"));
            client.print (nCount);
            client.printf ("\t%-8s", CorePartition_GetThreadNameByID (nCount));
            client.print (F ("\t"));
            client.print (CorePartition_GetStatusByID (nCount));
            client.print (CorePartition_IsSecureByID (nCount));
            client.print (F ("\t"));
            client.print (CorePartition_GetNiceByID (nCount));
            client.print (F ("\t"));
            client.print (CorePartition_GetStackSizeByID (nCount));
            client.print (F ("\t"));
            client.print (CorePartition_GetMaxStackSizeByID (nCount));
            client.print (F ("\t"));
            client.print (CorePartition_GetThreadContextSize ());
            client.print (F ("\t"));
            client.print (CorePartition_GetMaxStackSizeByID (nCount) + CorePartition_GetThreadContextSize ());
            client.print (F ("\t"));
            client.print (CorePartition_GetLastDutyCycleByID (nCount));
            client.println ("ms");
            client.flush ();
            CorePartition_Sleep (0);
        }
    }
}

#endif