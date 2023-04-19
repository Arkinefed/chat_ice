#include "Chat2I.h"

#include <random>

// LobbyI
Chat::LobbyI::LobbyI()
{
}

void Chat::LobbyI::_cpp_register(::std::shared_ptr<UserPrx> user,
                                 ::std::string password,
                                 const Ice::Current &current)
{
    if (m_users.count(user->getName()))
    {
        throw UserExists();
    }

    m_users.emplace(user->getName(), password);

    std::cout << "user registered: " << user->getName() << '\n';
}

::std::string
Chat::LobbyI::login(::std::shared_ptr<UserPrx> user,
                    ::std::string password,
                    const Ice::Current &current)
{
    if (m_users.find(user->getName()) != m_users.end())
    {
        for (auto u : m_loggedUsers)
        {
            try
            {
                if (u.first->getName() == user->getName())
                {
                    throw AccessDenied();
                }
            }
            catch (std::exception e)
            {
            }
        }

        if (m_users[user->getName()] == password)
        {
            std::string token = generateToken();

            m_loggedUsers.emplace(user, token);

            std::cout << "user logged: " << user->getName() << '\n';

            return token;
        }
    }

    throw AccessDenied();
}

void Chat::LobbyI::logout(const Ice::Current &current)
{
    std::string name;
    std::string token;

    for (auto c : current.ctx)
    {
        if (c.first == "name")
        {
            name = c.second;
        }
        else if (c.first == "token")
        {
            token = c.second;
        }
    }

    for (auto u : m_loggedUsers)
    {
        try
        {
            if (u.first->getName() == name && token == u.second)
            {
                auto it = std::find(m_loggedUsers.begin(), m_loggedUsers.end(), u);

                std::cout << "user logged out: " << u.first->getName() << '\n';

                m_loggedUsers.erase(it);

                return;
            }
        }
        catch (std::exception e)
        {
        }
    }

    throw AccessDenied();
}

::Chat::Rooms
Chat::LobbyI::getRooms(const Ice::Current &current)
{
    // AccessDenied

    return m_rooms;
}

::Chat::Users
Chat::LobbyI::getUsers(const Ice::Current &current)
{
    Users users;

    for (auto u : m_loggedUsers)
    {
        users.push_back(u.first);
    }

    return users;
}

::std::shared_ptr<::Chat::RoomPrx>
Chat::LobbyI::createRoom(::std::string name,
                         const Ice::Current &current)
{
    // AccessDenied

    for (auto r : m_rooms)
    {
        if (r->getName() == name)
        {
            throw RoomExists();
        }
    }

    auto obj = current.adapter->addWithUUID(std::make_shared<RoomI>(name));
    auto prx = Ice::uncheckedCast<RoomPrx>(obj);

    m_rooms.push_back(prx);

    return prx;
}

::std::shared_ptr<::Chat::RoomPrx>
Chat::LobbyI::findRoom(::std::string name,
                       const Ice::Current &current)
{
    // AccessDenied

    for (auto r : m_rooms)
    {
        if (name == r->getName())
        {
            return r;
        }
    }

    throw NoSuchRoom();
}

void Chat::LobbyI::registerRoomFactory(::std::shared_ptr<RoomFactoryPrx> roomFactory,
                                       const Ice::Current &current)
{
    for (auto f : m_roomFactories)
    {
        if (f == roomFactory)
        {
            throw RoomFactoryExists();
        }
    }

    m_roomFactories.push_back(roomFactory);
}

void Chat::LobbyI::unregisterRoomFactory(::std::shared_ptr<RoomFactoryPrx> roomFactory,
                                         const Ice::Current &current)
{
    for (auto it = m_roomFactories.begin(); it != m_roomFactories.end(); it++)
    {
        if (*it == roomFactory)
        {
            m_roomFactories.erase(it);

            return;
        }
    }

    throw NoSuchRoomFactory();
}

std::string Chat::LobbyI::generateToken()
{
    std::random_device engine;
    std::uniform_int_distribution<int> distribution(0, 25);

    std::string token;

    for (int i = 0; i < 10; i++)
    {
        token += 'a' + distribution(engine);
    }

    return token;
}

// UserI
Chat::UserI::UserI(std::string name, UserStatus userStatus)
    : m_name(name), m_userStatus(userStatus)
{
}

::std::string
Chat::UserI::getName(const Ice::Current &current)
{
    return m_name;
}

::Chat::UserStatus
Chat::UserI::getStatus(const Ice::Current &current)
{
    return m_userStatus;
}

void Chat::UserI::receivePrivateMessage(::std::shared_ptr<UserPrx> fromUser,
                                        ::std::string message,
                                        const Ice::Current &current)
{
    std::cout << "private message\n\t" << fromUser->getName() << ": " << message << '\n';
}

void Chat::UserI::receiveMessage(::std::shared_ptr<RoomPrx> fromRoom,
                                 ::std::shared_ptr<UserPrx> fromUser,
                                 ::std::string message,
                                 const Ice::Current &current)
{
    std::cout << fromRoom->getName() << "\n\t" << fromUser->getName() << ": " << message << '\n';
}

// RoomI
Chat::RoomI::RoomI(std::string name)
    : m_name(name)
{
}

::std::string
Chat::RoomI::getName(const Ice::Current &current)
{
    return m_name;
}

::Chat::Users
Chat::RoomI::getUsers(const Ice::Current &current)
{
    return m_users;
}

void Chat::RoomI::join(::std::shared_ptr<UserPrx> user,
                       const Ice::Current &current)
{
    for (auto u : m_users)
    {
        if (u->getName() == user->getName())
        {
            return;
        }
    }

    m_users.push_back(user);
}

void Chat::RoomI::leave(::std::shared_ptr<UserPrx> user,
                        const Ice::Current &current)
{
    for (auto u : m_users)
    {
        if (u->getName() == user->getName())
        {
            auto it = std::find(m_users.begin(), m_users.end(), u);

            m_users.erase(it);

            return;
        }
    }
}

void Chat::RoomI::sendMessage(::std::shared_ptr<UserPrx> fromUser,
                              ::std::string message,
                              const Ice::Current &current)
{
    auto prx = Ice::uncheckedCast<RoomPrx>(current.adapter->createProxy(current.id));

    for (auto u : m_users)
    {
        if (u->getName() != fromUser->getName())
        {
            u->receiveMessage(prx, fromUser, message, current.ctx);
        }
    }
}

// RoomFactoryI
Chat::RoomFactoryI::RoomFactoryI()
    : m_serverLoad(0)
{
}

double
Chat::RoomFactoryI::getServerLoad(const Ice::Current &current)
{
    return m_serverLoad;
}

::std::shared_ptr<::Chat::RoomPrx>
Chat::RoomFactoryI::createRoom(::std::string name,
                               const Ice::Current &current)
{
    // RoomExists

    return nullptr;
}
