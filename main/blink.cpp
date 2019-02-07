#include "MainBLEServer.h"

extern "C" void app_main()
{
    MainBLEServer* pMainBleServer = new MainBLEServer();
    pMainBleServer->start();
}
