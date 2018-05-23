#include <Ice/Ice.h>
#include <Chat.h>
#include <time.h>
#include "Impl/UserImpl.h"

using namespace std;
using namespace Chat;

class ClientHandler{

private:
    Ice::CommunicatorPtr ic;
    ServerPrx serverPrx;
    RoomPrx roomPrx;
    UserPrx userPrx;
    string userName;
    string password;
    int port;

    void initialize(){
        this->port = randPort();
        ic = Ice::initialize();
        Ice::ObjectPrx base = ic->stringToProxy(
                "Server:default -p 10000");
        serverPrx = ServerPrx::checkedCast(base);
        if (!serverPrx)
            throw "Invalid proxy";
    }

    int randPort(){
        srand (time(NULL));
        return rand() % 1000 + 10000;
    }

    string sgetPort(){
        ostringstream ss;
        ss << port;
        return ss.str();
    }

public:
    ClientHandler(){
        initialize();
    }

    ~ClientHandler(){
        if(roomPrx){
            roomPrx->LeaveRoom(userPrx, password);
            serverPrx->removeUser(userPrx);
            cout << "You have been log out successfully!" << endl;
            ic->destroy();
        }
    }

    Ice::CommunicatorPtr getCommunicatorPtr(){
        return ic;
    }

    string getUserName(){
        return userName;
    }

    string getRoomName(){
        return roomPrx->getName();
    }

    bool isRoom(){
        if(roomPrx){
            return true;
        } else {
            return false;
        }
    }

    bool createUser(string userName, string password){
        try{
            serverPrx->FindUser(userName);
        } catch(NoSuchUserExists e){
            this->userName = userName;
            this->password = password;


            cout << "Port: " << endl;
            cout << sgetPort() << endl;

            Ice::ObjectAdapterPtr adapter
                    = ic->createObjectAdapterWithEndpoints(userName, "default -p " + sgetPort());
            adapter->activate();



            UserPtr userPtr = new UserImpl(userName, password);
            userPrx = UserPrx::uncheckedCast(adapter->addWithUUID(userPtr));
            serverPrx->RegisterUser(userPrx);
            return true;
        }
        return false;
    }

    bool createRoom(string roomName){

        try{
            serverPrx->FindRoom(roomName);
        } catch(NoSuchRoomExists e){

            if(roomPrx){
                roomPrx->LeaveRoom(this->userPrx, this->password);
            }

            roomPrx = serverPrx->CreateRoom(roomName);
            roomPrx->AddUser(userPrx, this->password);
            return true;
        }
        return false;
    }

    bool joinRoom(string roomName){
        RoomPrx joinRoomPrx;
        try{
            joinRoomPrx = serverPrx->FindRoom(roomName);
        }catch(NoSuchRoomExists e){
            return false;
        }

        if(roomPrx){
            roomPrx->LeaveRoom(this->userPrx, this->password);
        }

        roomPrx = joinRoomPrx;
        roomPrx->AddUser(this->userPrx, this->password);
        return true;
    }

    void say(string message){
        roomPrx->SendMessage(userPrx, message, this->password);
    }

    void whisper(string userNameDestination, string message){
        try{
            UserPrx destinationUserPrx = serverPrx->FindUser(userNameDestination);
            destinationUserPrx->SendPrivateMessage(this->userPrx, message);
        }catch(NoSuchUserExists e){
            cout << "Cannot send message - user does not exists!" << endl;
        }
    }
    void listUsers(){
        if(roomPrx){
            UserList userList = roomPrx->getUsers();
            cout << "Users in room: " << endl;
            for(UserList::iterator it = userList.begin(); it != userList.end(); ++it) {
                cout << (*it)->getName() << endl;
            }            
        }
        else{
            cout << "You are not in the room" << endl;
        }
    }
    void leave(){
        RoomPrx leaveRoomPrx;
        if(roomPrx){
            roomPrx->LeaveRoom(this->userPrx, this->password);
            cout << "You leave room correctly" << endl;
        }
        else{
            cout << "You are not in the room" << endl;
        }
        roomPrx = leaveRoomPrx;
    }

};

class commandSplitter{
private:
    string key;
    string args;

public:

    string getKey(){
        return key;
    }

    string getArgs(){
        return args;
    }

    void execute(string command){
        this->clear();
        int position = 0;
        char current;

        while(true){
            current = command[position];
            if(current == ' ' || position >= command.length()){
                break;
            }
            position++;
            key += current;
        }

        position++;

        for(int i = position; i < command.length(); i++){
            current = command[position++];
            args += current;
        }
    }

    void clear(){
        key = "";
        args = "";
    }
};


int
main(int argc, char* argv[])
{
    int status = 0;
    Ice::CommunicatorPtr ic;
    try {
        ClientHandler client;
        ic = client.getCommunicatorPtr();
        string userName, password;

        cout << endl;

        bool isRegistered;
        do {
            cout << "Register user" << endl;
            cout << "User name: " << endl;
            cin >> userName;
            cout << "Password" << endl;
            cin >> password;
            cout << endl;

            isRegistered = client.createUser(userName, password);
            if(!isRegistered){
                cout << "Username is not available!" << endl;
            }
        } while(!isRegistered);


        cout << endl;
        cout << "You are now registered as " + client.getUserName() << endl;

        commandSplitter commandSplitter;
        string commandInput;
        string key;
        string args;

        cin.ignore();
        while(true){
            cout << endl;

            getline(cin, commandInput);
            if(commandInput == "/exit"){
                break;
            }

            commandSplitter.execute(commandInput);
            key = commandSplitter.getKey();
            args = commandSplitter.getArgs();

            if(key == "/createroom"){
                if(client.createRoom(commandSplitter.getArgs())){
                    cout << "Room " << args << " has been created successfully!" << endl;
                    cout << "You are now in the room: " << client.getRoomName() << endl;
                } else {
                    cout << "Can't create room " << args << ". Room already exists!" <<endl;
                }
                continue;
            }

            if(key == "/say"){
                if(client.isRoom()){
                    client.say(args);
                }          
                else{
                    cout << "You have to join to the room to say something" << endl;
                }
                continue;
            }

            if(key == "/joinroom"){
                if(client.joinRoom(args)){
                    cout << "You have joined room " + args + " successfully!" << endl;
                } else {
                    cout << "Can't join room. Check if room exists!" << endl;
                }
                continue;
            }

            if(key == "/whisper"){
                string userNameDestination = args;
                string message;
                cout << "Write message:" << endl;
                getline(cin, message);
                client.whisper(userNameDestination, message);
                continue;
            }

            if(key == "/leave"){
                client.leave();
                continue;
            }
            if(key == "/users")
            {
                if(client.isRoom()){
                    client.listUsers();
                    
                }          
                else{
                    cout << "You have to join to the room to list users" << endl;
                }
                continue;
            }

            if(key == "/help"){
                cout << endl;
                cout << "===== HELP ===== "<< endl;
                cout << endl;
                cout << "/createroom : creating new instance of room "<< endl;
                cout << "/say : sending message to current room" << endl;
                cout << "/whisper : sending message to user. First write command and after that you will be able to write message! " << endl;
                cout << "/leave : leave current room" << endl;
                continue;
            }

            cout << "Command not found" << endl;
        }



    } catch (const Ice::Exception& ex) {
        cerr << ex << endl;
        status = 1;
    } catch (const char* msg) {
        cerr << msg << endl;
        status = 1;
    }
    if (ic)
        ic->destroy();

    cout << "Exit program..." << endl;
    return status;
}
