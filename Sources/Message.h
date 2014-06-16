#pragma once

#include <SDL_net.h>
#include <string>

struct Message
{
    Message();
    explicit Message(const std::string& text);
    std::string text;

    int from;
    int to;

    std::string type;
    int message_number;
};

bool SocketReady(TCPsocket& socket);

bool SendSocketMessage(TCPsocket& socket, const Message& message);
bool RecvSocketMessage(TCPsocket& socket, Message* message);