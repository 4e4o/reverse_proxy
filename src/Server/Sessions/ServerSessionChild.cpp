#include "ServerSessionChild.hpp"
#include "ServerSession.hpp"

ServerSessionChild::ServerSessionChild(ServerSession* s)
    : Session(s), ServerHandshake(this, s) {
}
