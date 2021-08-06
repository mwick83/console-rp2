// Console IO is a wrapper between the actual in and output and the console code
// In an embedded system, this might interface to a UART driver.

#include "consoleIo.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <stdio.h>
#include "pico/stdlib.h"

#include "elog.h"

// prototype from console.c to prevent circular include in the first place
extern void ConsoleProcess(void);


// private globals
static bool consoleIoEchoEn;


eConsoleError ConsoleIoInit(void)
{
    consoleIoEchoEn = false;
    stdio_flush();
    return CONSOLE_SUCCESS;
}

eConsoleError ConsoleIoSetEcho(bool enable)
{
    consoleIoEchoEn = enable;
    return CONSOLE_SUCCESS;
}

eConsoleError ConsoleIoReady(void)
{
    eConsoleError ret = CONSOLE_ERROR;

    // TBD: this drops the received char. live with it?
    int rc = getchar_timeout_us(0);
    if ((rc > 0) && (rc < 128)) {
        ret = CONSOLE_SUCCESS;
    }

    return ret;
}

eConsoleError ConsoleIoReceive(uint8_t *buffer, const uint32_t bufferLength, uint32_t *readLength)
{
    uint8_t i = 0;

    while (i < bufferLength) {
        int rc = getchar_timeout_us(0);

        if (rc >= 0) {
            buffer[i] = (uint8_t) rc;
            i++;

            if (consoleIoEchoEn) {
                putchar(rc);
            }
        } else {
            break;
        }
    }
    *readLength = i;
    return CONSOLE_SUCCESS;
}

eConsoleError ConsoleIoSend(const uint8_t *buffer, const uint32_t bufferLength, uint32_t *sentLength)
{
    printf("%.*s", bufferLength, (char*) buffer);
    *sentLength = bufferLength;
    return CONSOLE_SUCCESS;
}

eConsoleError ConsoleIoSendString(const char *buffer)
{
    printf("%s", buffer);
    return CONSOLE_SUCCESS;
}
