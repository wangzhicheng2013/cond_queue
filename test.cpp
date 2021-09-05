#include "cond_queue.hpp"
cond_queue<std::size_t> g_queue;
const std::size_t count = 1000;
void pusher() {
    for (std::size_t i = 0;i < count;++i) {
        g_queue.push(i);
    }
}
void popper() {
    std::size_t e = 0;
    for (std::size_t i = 0;i < count;++i) {
        g_queue.pop(e);
    }
}
int main() {
    boost::thread pop1(&popper);
    boost::thread pop2(&popper);
    boost::thread pop3(&popper);
    
    boost::thread push1(&pusher);
    boost::thread push2(&pusher);
    boost::thread push3(&pusher);

    
    push1.join();
    push2.join();
    push3.join();
    g_queue.flush();

    pop1.join();
    pop2.join();
    pop3.join();

    return 0;
}