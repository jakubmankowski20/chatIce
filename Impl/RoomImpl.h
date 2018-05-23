#ifndef ROOM_IMPL_H
    #define ROOM_IMPL_H

    #include <Ice/Ice.h>
    #include <Chat.h>

    #include "UserImpl.h"
    #include "ServerImpl.h"

    using namespace std;
    using namespace Chat;

    class RoomImpl : public Room {
    private:
        string name;
        UserList userList;
        ServerPrx serverPrx;
    public:
        RoomImpl(string name, ServerPrx serverPrx);
        virtual string getName(const ::Ice::Current&);
        virtual UserList getUsers(const ::Ice::Current&);
        virtual void AddUser(const UserPrx& user,
                             const string& password,
                             const ::Ice::Current&);
        virtual void SendMessage(const UserPrx& user,
                                 const string& message,
                                 const string& password,
                                 const Ice::Current&);
        virtual void Destroy(const Ice::Current&);
        virtual void LeaveRoom(const UserPrx& user,
                               const string& password,
                               const Ice::Current&);

    };

#endif