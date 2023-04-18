#ifndef __Chat2I_h__
#define __Chat2I_h__

#define ICE_CPP11_MAPPING

#include "Chat2.h"
#include <Ice/Ice.h>

namespace Chat
{
    class LobbyI : public virtual Lobby
    {
    public:
        LobbyI();

        virtual void _cpp_register(::std::shared_ptr<UserPrx>,
                                   ::std::string,
                                   const Ice::Current &) override;

        virtual ::std::string login(::std::shared_ptr<UserPrx>,
                                    ::std::string,
                                    const Ice::Current &) override;

        virtual void logout(const Ice::Current &) override;

        virtual Rooms getRooms(const Ice::Current &) override;

        virtual Users getUsers(const Ice::Current &) override;

        virtual ::std::shared_ptr<RoomPrx> createRoom(::std::string,
                                                      const Ice::Current &) override;

        virtual ::std::shared_ptr<RoomPrx> findRoom(::std::string,
                                                    const Ice::Current &) override;

        virtual void registerRoomFactory(::std::shared_ptr<RoomFactoryPrx>,
                                         const Ice::Current &) override;

        virtual void unregisterRoomFactory(::std::shared_ptr<RoomFactoryPrx>,
                                           const Ice::Current &) override;

    private:
        std::vector<RoomPrxPtr> m_rooms;
        std::map<std::string, std::string> m_users;
        std::map<UserPrxPtr, std::string> m_loggedUsers;
        std::vector<RoomFactoryPrxPtr> m_roomFactories;
    };

    class UserI : public virtual User
    {
    public:
        UserI(std::string name, UserStatus userStatus);

        virtual ::std::string getName(const Ice::Current &) override;

        virtual UserStatus getStatus(const Ice::Current &) override;

        virtual void receivePrivateMessage(::std::shared_ptr<UserPrx>,
                                           ::std::string,
                                           const Ice::Current &) override;

        virtual void receiveMessage(::std::shared_ptr<RoomPrx>,
                                    ::std::shared_ptr<UserPrx>,
                                    ::std::string,
                                    const Ice::Current &) override;

    private:
        std::string m_name;
        UserStatus m_userStatus;
    };

    class RoomI : public virtual Room
    {
    public:
        RoomI(std::string name);

        virtual ::std::string getName(const Ice::Current &) override;

        virtual Users getUsers(const Ice::Current &) override;

        virtual void join(::std::shared_ptr<UserPrx>,
                          const Ice::Current &) override;

        virtual void leave(::std::shared_ptr<UserPrx>,
                           const Ice::Current &) override;

        virtual void sendMessage(::std::shared_ptr<UserPrx>,
                                 ::std::string,
                                 const Ice::Current &) override;

    private:
        std::string m_name;
        std::vector<UserPrxPtr> m_users;
    };

    class RoomFactoryI : public virtual RoomFactory
    {
    public:
        RoomFactoryI();

        virtual double getServerLoad(const Ice::Current &) override;

        virtual ::std::shared_ptr<RoomPrx> createRoom(::std::string,
                                                      const Ice::Current &) override;

    private:
        double m_serverLoad;
    };
}

#endif
