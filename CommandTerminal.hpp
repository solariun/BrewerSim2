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

#ifndef COMMAND_TERMINAL_HPP
#define COMMAND_TERMINAL_HPP

#include <ESP8266WiFi.h>
#include <user_interface.h>

#include "Arduino.h"

#include <assert.h>
#include <functional>

#include "CorePartition.h"
#include "Util.hpp"
#include "Terminal.hpp"


class TStream : public TerminalStream
{
public:
    TStream (Stream& client) : TerminalStream (client)
    {
    }

    bool IsConnected () override
    {
        return true;
    }

    bool Disconnect () override
    {
        return true;
    }

    void ContextSwitch () override
    {
        CorePartition_Yield ();
    }
};

TStream tstream = TStream (Serial);

class StatusCommand : public TerminalCommand
{
public:
    StatusCommand ()
    {
    }

    bool Execute (Terminal& terminal, TerminalStream& client, const String& strCommandLine)
    {
        uint8_t nNumCommands;
        String strOption;

        if ((nNumCommands = ParseOption (strCommandLine, 1, strOption, true)) > 0)
        {
            if (nNumCommands > 2)
            {
                client().printf ("Warning: detected more options (%u) than necessary, aborting. (%s)\n", nNumCommands, strOption.c_str());
                HelpMessage (client);

                return false;
            }

            if (strOption == "threads")
            {
                ShowRunningThreads (client());
            }
            else if (strOption == "memory")
            {
                client().print ("free heap: ");
                client().print (system_get_free_heap_size () / 1024);
                client().println ("Kb");
            }
            else if (strOption == "system")
            {
                client().println ("ESP8266 System ------------------");
                client().printf ("%-20s: [%u]\r\n", "Processor ID", system_get_chip_id ());
                client().printf ("%-20s: [%s]\r\n", "SDK Version", system_get_sdk_version ());
                client().printf ("%-20s: [%uMhz]\r\n", "CPU Freequency", system_get_cpu_freq ());
                client().printf ("%-20s: [%u Bytes]\r\n", "Memory", system_get_free_heap_size ());
                // if (WiFi.status () != WL_NO_SHIELD)
                // {
                //     client().println ("-[Connection]----------------------");
                //     client().printf ("%-20s: [%s] (%u)\r\n", "Status", GetWiFiStatus (), WiFi.status ());
                //     client().printf ("%-20s: [%s]\r\n", "Mac Address", WiFi.macAddress ().c_str ());
                //     if (WiFi.status () == WL_CONNECTED)
                //     {
                //         client().printf ("%-20s: [%s]\r\n", "SSID", WiFi.SSID ().c_str ());
                //         client().printf ("%-20s: [%d dBm]\r\n", "Signal", WiFi.RSSI ());
                //         client().printf ("%-20s: [%u Mhz]\r\n", "Channel", WiFi.channel ());
                //         client().printf ("%-20s: [%s]\r\n", "IPAddress", WiFi.localIP ().toString ().c_str ());
                //         client().printf ("%-20s: [%s]\r\n", "Net Mask", WiFi.subnetMask ().toString ().c_str ());
                //         client().printf ("%-20s: [%s]\r\n", "Gateway", WiFi.gatewayIP ().toString ().c_str ());
                //         client().printf ("%-20s: [%s]\r\n", "DNS", WiFi.dnsIP ().toString ().c_str ());
                //     }
                // }
                client().println ("-[Process]----------------------");
                ShowRunningThreads (client());
            }
            else
            {
                client().printf ("Error, invalid option: [%s]\n", strOption.c_str());
                HelpMessage (client);
            }
        }

        return true;
    }

    void HelpMessage (TerminalStream& client)
    {
        client ().println ("Show status of the terminal");
        client ().println ("\tUse:\nstatus thread|memory|system");   
        client ().println ("");
    }
};

StatusCommand statusCommand;

void MOTDFunction (TerminalStream& stdio)
{
    stdio ().println ("---------------------------------");
    stdio ().println ("Embedded Terminal");
    stdio ().println (TERM_VERSION "." TERM_SUBVERSION "." TERM_STAGE);
    stdio ().println ("---------------------------------");
}

void Thread_Serial (void* pValue)
{
    Serial.begin (115200);

    while (true)
    {
        while (!Serial)
        {
            CorePartition_Yield ();
        }

        Terminal terminal (tstream);

        Serial.println ("Terminal Demo");

        terminal.AttachMOTD (MOTDFunction);

        terminal.AttachCommand ("Status", statusCommand);

        terminal.Start ();
    }
}

#endif