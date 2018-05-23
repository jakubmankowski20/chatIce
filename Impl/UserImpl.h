#ifndef USER_IMPL_H
    #define USER_IMPL_H
    #include <Ice/Ice.h>
    #include <Chat.h>

    #include "RoomImpl.h"

    using namespace std;
    using namespace Chat;

    class UserImpl : public User{
    private:
        string password;
        string userName;
    public:
        UserImpl(const string& userName, const string& password);

        virtual void SendMessage(const string&,
                                 const UserPrx& who,
                                 const string& message,
                                 const Ice::Current&);
        virtual void SendPrivateMessage(const UserPrx& who,
                                        const string& message,
                                        const Ice::Current&);
        virtual string getName(const Ice::Current&);

        virtual void ChangePassword(const string& oldpassword,
                                    const string& newpassword,
                                    const Ice::Current&);
        virtual string getPassword(const Ice::Current&);
    };

#endif
