module Chat {

interface Room;

exception UserAlreadyExists {};
exception RoomAlreadyExists {};
exception NoSuchUserExists {};
exception NoSuchRoomExists {};
exception WrongPassword {};


interface User {
  void SendMessage(string roomName, User* who, string message);
  void SendPrivateMessage(User* who, string message);
  void ChangePassword(string oldpassword, string newpassword) throws WrongPassword;
  string getName();
  string getPassword();
};

sequence<Room*> RoomList;

interface Server {
  Room* CreateRoom(string name) throws RoomAlreadyExists;
  RoomList getRooms();
  Room* FindRoom(string name) throws NoSuchRoomExists;
  User* FindUser(string name) throws NoSuchUserExists;
  void RegisterUser(User* name) throws UserAlreadyExists;
  void removeUser(User* name) throws NoSuchUserExists;
};

sequence<User*> UserList;

interface Room {
  string getName();
  UserList getUsers();
  void AddUser(User* who, string password) throws UserAlreadyExists;
  void SendMessage(User* who, string message, string passwd);
  void Destroy();
  void LeaveRoom(User* who, string passwd);
};


};
