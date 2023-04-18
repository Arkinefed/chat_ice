#include <Ice/Ice.h>
#include "ice/cpp/Chat2I.h"

int main(int argc, char **argv)
{
    try
    {
        Ice::CommunicatorHolder ich(argc, argv);

        auto adapter = ich->createObjectAdapterWithEndpoints("LobbyAdapter", "default -p 10000");

        adapter->add(std::make_shared<Chat::LobbyI>(), Ice::stringToIdentity("Lobby"));
        adapter->activate();

        ich->waitForShutdown();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        
        return 1;
    }

    return 0;
}