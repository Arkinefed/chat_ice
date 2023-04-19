#include <Ice/Ice.h>
#include "ice/cpp/Chat2I.h"

auto inputCredentials() -> std::pair<std::string, std::string>;

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

        auto userAdapter = ich->createObjectAdapterWithEndpoints("User", "default");
        userAdapter->activate();

        auto credentials = inputCredentials();

        auto servant = std::make_shared<Chat::UserI>(credentials.first, Chat::UserStatus::Online);
        auto user = Ice::uncheckedCast<Chat::UserPrx>(userAdapter->addWithUUID(servant));

        std::cout << "l - login, r - register, other - exit" << std::endl;

        std::string option;

        std::cin >> option;

        std::string token;

        bool registered;

        if (option == "l")
        {
            try
            {
                token = lobby->login(user, credentials.second);

                std::cout << "logged in\n";

                while (true)
                {
                    std::cout << "commands\n";
                    std::cout << "\tlogout\n";
                    std::cout << "\tgetRooms\n";
                    std::cout << "\tgetUsers\n";
                    std::cout << "\tcreateRoom\n";
                    std::cout << "\tfindRoom\n";

                    std::cin >> option;

                    if (option == "logout")
                    {
                        Ice::Context ctx;

                        ctx.emplace("name", user->getName());
                        ctx.emplace("token", token);

                        lobby->logout(ctx);

                        return 0;
                    }
                    else if (option == "getRooms")
                    {
                    }
                    else if (option == "getUsers")
                    {
                    }
                    else if (option == "createRoom")
                    {
                    }
                    else if (option == "findRoom")
                    {
                    }
                }
            }
            catch (Chat::AccessDenied e)
            {
                std::cout << "failed to log in\n";

                return 1;
            }
        }
        else if (option == "r")
        {
            registered = false;

            try
            {
                lobby->_cpp_register(user, credentials.second);

                std::cout << "registered\n";

                registered = true;
            }
            catch (Chat::UserExists e)
            {
                std::cout << "user exists\n";

                return 1;
            }
        }
        else
        {
            return 0;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;

        return 1;
    }

    return 0;
}

auto inputCredentials() -> std::pair<std::string, std::string>
{
    std::string login;
    std::string password;

    std::cout << "login: ";
    std::cin >> login;

    std::cout << "password: ";
    std::cin >> password;

    return std::make_pair(login, password);
}