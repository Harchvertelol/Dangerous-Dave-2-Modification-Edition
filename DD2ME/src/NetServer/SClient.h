#ifndef SCLIENT_H
#define SCLIENT_H

#include "../shabbynet/shabbynet.hpp"

class UserData;
class Server;

class SClient: public Socket
{
    public:
        SClient(Server*);
        ~SClient();
        int s_ID;
        Server* s_Server;
        UserData* s_UserData;
        std::string s_ReceiveBuffer;

    private:
        void on_received(const char*, int);
        void on_connected();
        void on_closed();
};

#endif // SCLIENT_H
