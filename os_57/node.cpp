#include <iostream>
#include <string>
#include <chrono>
#include <zmq.hpp>

class Timer {
private:
    bool running = false;
    std::chrono::steady_clock::time_point start_time;
    std::chrono::milliseconds elapsed{0};

public:
    void start() {
        if (!running) {
            running = true;
            start_time = std::chrono::steady_clock::now();
        }
    }

    void stop() {
        if (running) {
            elapsed += std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start_time);
            running = false;
        }
    }

    long long time() {
        if (running) {
            return elapsed.count() + std::chrono::duration_cast<std::chrono::milliseconds>(
                                         std::chrono::steady_clock::now() - start_time)
                                         .count();
        }
        return elapsed.count();
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Error: Invalid arguments\n";
        return 1;
    }

    int id = std::stoi(argv[1]);
    Timer timer;

    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    std::ostringstream endpoint;
    endpoint << "tcp://*:" << 6666 + id;
    socket.bind(endpoint.str());

    std::cout << "Node " << id << " started\n";

    while (true) {
        zmq::message_t request;
        zmq::recv_result_t recv_result = socket.recv(request, zmq::recv_flags::none);
        std::string cmd(static_cast<char*>(request.data()), request.size());

        std::string response;
        if (cmd == "start") {
            timer.start();
            response = "Ok: start";
        } else if (cmd == "stop") {
            timer.stop();
            response = "Ok: stop";
        } else if (cmd == "time") {
            response = "Ok: time " + std::to_string(timer.time()) + "ms";
        } else {
            response = "Error: Unknown command";
        }

        zmq::message_t reply(response.size());
        memcpy(reply.data(), response.data(), response.size());
        socket.send(reply, zmq::send_flags::none);
    }

    return 0;
}