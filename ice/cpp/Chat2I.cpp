#include "Chat2I.h"

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
        if (m_loggedUsers.find(user->getName()) != m_loggedUsers.end())
        {
            throw AccessDenied();
        }

        if (m_users[user->getName()] == password)
        {
            std::string token = "logged";

            m_loggedUsers.emplace(user->getName(), token);

            std::cout << "user logged: " << user->getName() << '\n';

            return token;
        }
    }

    throw AccessDenied();
}

void Chat::LobbyI::logout(const Ice::Current &current)
{
    // auto userPrx = Ice::uncheckedCast<UserPrx>(current.adapter->createProxy(current.id));

    // for (auto u : m_loggedUsers)
    // {
    //     if (u->getName() == userPrx->getName())
    //     {
    //         auto it = std::find(m_loggedUsers.begin(), m_loggedUsers.end(), u);

    //         m_loggedUsers.erase(it);

    //         return;
    //     }
    // }

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
    return Users();
    // return m_users;
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