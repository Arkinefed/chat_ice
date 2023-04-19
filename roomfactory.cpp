#include <Ice/Ice.h>
#include "ice/cpp/Chat2I.h"

int main(int argc, char **argv)
{
    try
    {
        Ice::CommunicatorHolder ich(argc, argv);

        ich->waitForShutdown();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;

        return 1;
    }

    return 0;
}