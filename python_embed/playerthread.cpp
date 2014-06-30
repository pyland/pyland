#include <thread>
#include "api.h"
#include "playerthread.h"

PlayerThread::PlayerThread(Player &player, std::unique_ptr<std::thread> thread, int64_t thread_id):
    player(player), thread(std::move(thread)), previous_call_number(0), thread_id(thread_id) {}

bool PlayerThread::is_dirty() {
    return previous_call_number != player.call_number;
}

void PlayerThread::set_clean() {
    previous_call_number = player.call_number;
}
