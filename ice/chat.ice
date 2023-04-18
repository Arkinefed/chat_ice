module Chat
{

exception AccessDenied {};
exception UserExists {};
exception RoomExists {};
exception RoomFactoryExists {};
exception NoSuchFactory {};

enum UserStatus {Online, Offline, BeBack};

interface Room;
interface User;
interface RoomFactory;

sequence<Room*> Rooms;
sequence<User*> Users;


interface Lobby
{
   void register(User* user, string password) throws UserExists;
   void login(User* user, string password) throws AccessDenied;
   void logout() throws AccessDenied;
   Rooms getRooms() throws AccessDenied;
   Room* createRoom(string name) throws AccessDenied, RoomExists;
   Room* findRoom(string name) throws AccessDenied;
   Users ListUsers();

   void RegisterRoomFactory(RoomFactory* factory) throws RoomFactoryExists;
   void UnregisterRoomFactory(RoomFactory* factory) throws NoSuchFactory;
}

interface User
{
   void sendMessage(Room* room, User* user, string message);
   void sendPrivateMessage(User* user, string message);
   string getName();
   UserStatus getStatus();
}


interface Room
{
  void join(User* user);
  void sendMessage(User* user, string message);
  void leave(User* user);
  Users ListUsers();
}

interface RoomFactory
{
  Room* createRoom(string name) throws RoomExists;
  double getServerLoad();
}

}