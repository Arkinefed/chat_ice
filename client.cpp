#include <Ice/Ice.h>
#include "ice/cpp/Chat2I.h"

auto inputCredentials();

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

        std::cout << "l - login, r - register, other - exit" << std::endl;

        std::string option;

        std::cin >> option;

        auto userAdapter = ich->createObjectAdapterWithEndpoints("User", "default -p 0");
        userAdapter->activate();

        if (option == "l")
        {
            auto credentials = inputCredentials();

            auto servant = std::make_shared<Chat::UserI>(credentials.first, Chat::UserStatus::Online);
            auto user = Ice::uncheckedCast<Chat::UserPrx>(userAdapter->addWithUUID(servant));

            try
            {
                std::string result = lobby->login(user, credentials.second);
            }
            catch (Chat::AccessDenied e)
            {
                std::cout << "failed to log in" << std::endl;
            }
        }
        else if (option == "r")
        {
            auto credentials = inputCredentials();

            auto servant = std::make_shared<Chat::UserI>(credentials.first, Chat::UserStatus::Online);
            auto user = Ice::uncheckedCast<Chat::UserPrx>(userAdapter->addWithUUID(servant));

            try
            {
                lobby->_cpp_register(user, credentials.second);
            }
            catch (Chat::UserExists e)
            {
                std::cout << "user exists" << std::endl;
            }
        }
        else
        {
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;

        return 1;
    }

    return 0;
}

auto inputCredentials()
{
    std::string login;
    std::string password;

    std::cout << "login: ";
    std::cin >> login;
    std::cout << "password: ";
    std::cin >> password;

    return std::make_pair(login, password);
}