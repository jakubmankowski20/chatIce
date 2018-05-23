#ifndef SERVER_IMPL_H
    #define SERVER_IMPL_H
    #include <Ice/Ice.h>
    #include <Chat.h>

    #include "RoomImpl.h"
    #include "UserImpl.h"

    using namespace Chat;
    using namespace std;

    class ServerImpl : public Server{
    private:
        RoomList roomList;
        UserList userList;
        Ice::ObjectAdapterPtr adapter;
    public:
        ServerImpl(Ice::ObjectAdapterPtr adapter);
        virtual RoomPrx CreateRoom(const string& name,
                                   const Ice::Current&);
        virtual RoomList getRooms(const Ice::Current&);
        virtual RoomPrx FindRoom(const string& name,
                                 const Ice::Current&);
        virtual void RegisterUser(const UserPrx&,
                                  const Ice::Current&);
        virtual UserPrx FindUser(const string& userName,
                                 const Ice::Current&);
        virtual void removeUser(const UserPrx& user,
                                const Ice::Current&);

    };

#endif