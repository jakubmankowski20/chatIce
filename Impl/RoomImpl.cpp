#include "RoomImpl.h"

RoomImpl::RoomImpl(string name, ServerPrx serverPrx){
    this->name = name;
    this->serverPrx = serverPrx;
}

string RoomImpl::getName(const Ice::Current&){
    return this->name;
}

UserList RoomImpl::getUsers(const ::Ice::Current&){
    return this->userList;
}

void RoomImpl::AddUser(const UserPrx& user,
                       const string& password,
                       const ::Ice::Current&){

    if(user->getPassword() != password){
        throw new WrongPassword;
    } else {
        userList.push_back(user);
        string message = user->getName() + "has joined the room!";
    }

}


void RoomImpl::SendMessage(const UserPrx& user,
                           const string& message,
                           const string& password,
                           const Ice::Current&){

    if(user->getPassword() != password){
        throw new WrongPassword;
    } else {
        RoomPrx roomPrx = serverPrx->FindRoom(this->name);
        string roomName = roomPrx->getName();
        for(vector<UserPrx>::iterator it = userList.begin(); it != userList.end(); ++it) {
            (*it)->SendMessage(roomName, user, message);
        }
    }
}

void RoomImpl::Destroy(const Ice::Current&){
    RoomList roomList = serverPrx->getRooms();
    roomList.erase(find(roomList.begin(), roomList.end(), serverPrx->FindRoom(name)));
}

void RoomImpl::LeaveRoom(const UserPrx& user,
                         const string& password,
                         const Ice::Current&){
    if(user->getPassword() != password){
        throw new WrongPassword;
    } else {
        userList.erase(find(userList.begin(), userList.end(),user));

        if(userList.size() == 0){
            serverPrx->FindRoom(name)->Destroy();
            cout << "Room deleted" << endl;     
        }
    }
}

