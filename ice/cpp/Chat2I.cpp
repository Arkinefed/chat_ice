#include "Chat2I.h"

#include "../../utils.h"

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
            std::string token = utils::generateToken();

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
                return m_rooms;
            }
        }
        catch (std::exception e)
        {
        }
    }

    throw AccessDenied();
}

::Chat::Users
Chat::LobbyI::getUsers(const Ice::Current &current)
{
    Users users;

    try
    {
        for (auto u : m_loggedUsers)
        {
            users.push_back(u.first);
        }
    }
    catch (std::exception e)
    {
    }

    return users;
}

::std::shared_ptr<::Chat::RoomPrx>
Chat::LobbyI::createRoom(::std::string name,
                         const Ice::Current &current)
{
    std::string username;
    std::string token;

    for (auto c : current.ctx)
    {
        if (c.first == "name")
        {
            username = c.second;
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
            if (u.first->getName() == username && token == u.second)
            {
                bool found = false;

                try
                {
                    for (auto r : m_rooms)
                    {
                        if (r->getName() == name)
                        {
                            found = true;
                        }
                    }
                }
                catch (std::exception e)
                {
                }

                if (found)
                {
                    throw RoomExists();
                }

                if (m_roomFactories.size() < 1)
                {
                    throw AccessDenied();
                }

                RoomFactoryPrxPtr rf = nullptr;

                for (auto f : m_roomFactories)
                {
                    try
                    {
                        double load = f->getServerLoad();

                        if (rf == nullptr)
                        {
                            rf = f;
                        }
                        else
                        {
                            if (load < rf->getServerLoad())
                            {
                                rf = f;
                            }
                        }
                    }
                    catch (std::exception e)
                    {
                    }
                }

                try
                {
                    auto room = rf->createRoom(name);
                    m_rooms.push_back(room);
                    return room;
                }
                catch (std::exception e)
                {
                }

                return nullptr;
            }
        }
        catch (std::exception e)
        {
        }
    }

    throw AccessDenied();
}

::std::shared_ptr<::Chat::RoomPrx>
Chat::LobbyI::findRoom(::std::string name,
                       const Ice::Current &current)
{
    std::string username;
    std::string token;

    for (auto c : current.ctx)
    {
        if (c.first == "name")
        {
            username = c.second;
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
            if (u.first->getName() == username && token == u.second)
            {
                for (auto r : m_rooms)
                {
                    try
                    {
                        if (name == r->getName())
                        {
                            return r;
                        }
                    }
                    catch (std::exception e)
                    {
                    }
                }

                throw NoSuchRoom();
            }
        }
        catch (std::exception e)
        {
        }
    }

    throw AccessDenied();
}

void Chat::LobbyI::registerRoomFactory(::std::shared_ptr<RoomFactoryPrx> roomFactory,
                                       const Ice::Current &current)
{
    bool found = false;

    for (auto f : m_roomFactories)
    {
        try
        {
            if (f->getName() == roomFactory->getName())
            {
                found = true;
            }
        }
        catch (std::exception e)
        {
        }
    }

    if (found)
    {
        throw RoomFactoryExists();
    }

    m_roomFactories.push_back(roomFactory);

    std::cout << "room factory " << roomFactory->getName() << " registered" << std::endl;
}

void Chat::LobbyI::unregisterRoomFactory(::std::shared_ptr<RoomFactoryPrx> roomFactory,
                                         const Ice::Current &current)
{
    for (auto it = m_roomFactories.begin(); it != m_roomFactories.end(); it++)
    {
        try
        {
            if (it->get()->getName() == roomFactory->getName())
            {
                std::cout << "room factory " << it->get()->getName() << " unregistered" << std::endl;

                m_roomFactories.erase(it);

                return;
            }
        }
        catch (std::exception e)
        {
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
    try
    {
        std::string u = fromUser->getName();

        std::cout << "private message\n\t" << u << ": " << message << '\n';
    }
    catch (std::exception e)
    {
    }
}

void Chat::UserI::receiveMessage(::std::shared_ptr<RoomPrx> fromRoom,
                                 ::std::shared_ptr<UserPrx> fromUser,
                                 ::std::string message,
                                 const Ice::Current &current)
{
    try
    {
        std::string r = fromRoom->getName();
        std::string u = fromUser->getName();

        std::cout << r << "\n\t" << u << ": " << message << '\n';
    }
    catch (std::exception e)
    {
    }
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
        try
        {
            if (u->getName() == user->getName())
            {
                return;
            }
        }
        catch (std::exception e)
        {
        }
    }

    m_users.push_back(user);
}

void Chat::RoomI::leave(::std::shared_ptr<UserPrx> user,
                        const Ice::Current &current)
{

    for (auto u : m_users)
    {
        try
        {
            if (u->getName() == user->getName())
            {
                auto it = std::find(m_users.begin(), m_users.end(), u);

                m_users.erase(it);

                return;
            }
        }
        catch (std::exception e)
        {
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
        try
        {
            if (u->getName() != fromUser->getName())
            {
                u->receiveMessageAsync(prx, fromUser, message, current.ctx);
            }
        }
        catch (std::exception e)
        {
        }
    }
}

// RoomFactoryI
Chat::RoomFactoryI::RoomFactoryI(std::string name)
    : m_name(name), m_serverLoad(0)
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

    auto servant = std::make_shared<Chat::RoomI>(name);
    auto room = Ice::uncheckedCast<Chat::RoomPrx>(current.adapter->addWithUUID(servant));

    if ((m_serverLoad += 1.0) >= 100.0)
    {
        m_serverLoad = 100.0;
    }

    std::cout << "room created: " << room->getName() << std::endl;

    return room;
}

::std::string
Chat::RoomFactoryI::getName(const Ice::Current &)
{
    return m_name;
}
