#include <Ice/Ice.h>
#include "ice/cpp/Chat2I.h"

int main(int argc, char **argv)
{
    try
    {
        Ice::CommunicatorHolder ich(argc, argv);

        auto lobby = Ice::checkedCast<Chat::LobbyPrx>(ich->stringToProxy("Lobby:default -p 10000"));

        if (!lobby)
        {
            throw std::runtime_error("Invalid proxy");
        }

        auto roomFactoryAdapter = ich->createObjectAdapterWithEndpoints("RoomFactory", "default");
        roomFactoryAdapter->activate();

        std::string name;

        std::cout << "room factory name: ";
        std::cin >> name;

        auto servant = std::make_shared<Chat::RoomFactoryI>(name);
        auto roomFactory = Ice::uncheckedCast<Chat::RoomFactoryPrx>(roomFactoryAdapter->addWithUUID(servant));

        lobby->registerRoomFactory(roomFactory);

        while (std::getchar() != '\n')
            ;

        while (true)
        {
            std::cout << "input quit to exit\n";
            std::cin >> name;

            if (name == "quit")
            {
                lobby->unregisterRoomFactory(roomFactory);

                return 0;
            }
        }

        ich->waitForShutdown();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;

        return 1;
    }

    return 0;
}