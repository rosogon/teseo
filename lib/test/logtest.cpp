#include <ArduinoUnitTests.h>
#include <Arduino.h>
#include "../log.h"


const size_t len = 80;
class TestConsole : public Print {

public:

    size_t write(uint8_t c) {
        if (nextReset) {
            reset();
        }
        printf("%c", c);
        *(buf + n++) = c;
        if (c == '\n') {
            nextReset = true;
        }
        if (n == len - 1) {
            reset();
        }
        return 1;
    }

    const char *buffer() {
        *(buf + n) = '\0';
        nextReset = true;
        return buf;
    }

private:
    char buf[len];
    bool nextReset = false;
    int n = 0;

    void reset() {
        nextReset = false;
        n = 0;
        *buf = '\0';
    }
};

unittest(test_log)
{
    TestConsole console;
    setConsole(console);

    console.println("Hello world!");
    int val = 3;
    tlog("main - ", "value=%d", val);
    assertEqual("0 - main - value=3\r\n", console.buffer());
}

static void _log(const char *format, ...) {
    LOG_VA("main - ", format);
}

unittest(test_vtlog)
{
    TestConsole console;
    setConsole(console);

    int val = 3;
    _log("value=%d", val);
    assertEqual("0 - main - value=3\r\n", console.buffer());
}

unittest_main()