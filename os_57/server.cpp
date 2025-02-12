#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <sys/wait.h>
#include <zmq.hpp>
#include <memory>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>

using namespace std;

struct NodeInfo {
    int id;             // Идентификатор узла
    int pid;            // PID процесса
    std::string endpoint; // Адрес ZeroMQ
};

template <typename Key, typename Value>
class AVLNode {
public:
    Key key;
    Value value;
    std::unique_ptr<AVLNode> left;
    std::unique_ptr<AVLNode> right;
    int height;

    AVLNode(Key k, Value v)
        : key(k), value(v), left(nullptr), right(nullptr), height(1) {}
};

template <typename Key, typename Value>
class AVLTree {
private:
    std::unique_ptr<AVLNode<Key, Value>> root;

    int height(const AVLNode<Key, Value>* node) const {
        return node ? node->height : 0;
    }

    int balanceFactor(const AVLNode<Key, Value>* node) const {
        return node ? height(node->left.get()) - height(node->right.get()) : 0;
    }

    std::unique_ptr<AVLNode<Key, Value>> rightRotate(std::unique_ptr<AVLNode<Key, Value>> y) {
        auto x = std::move(y->left);
        auto T2 = std::move(x->right);

        x->right = std::move(y);
        x->right->left = std::move(T2);

        x->right->height = std::max(height(x->right->left.get()), height(x->right->right.get())) + 1;
        x->height = std::max(height(x->left.get()), height(x->right.get())) + 1;

        return x;
    }

    std::unique_ptr<AVLNode<Key, Value>> leftRotate(std::unique_ptr<AVLNode<Key, Value>> x) {
        auto y = std::move(x->right);
        auto T2 = std::move(y->left);

        y->left = std::move(x);
        y->left->right = std::move(T2);

        y->left->height = std::max(height(y->left->left.get()), height(y->left->right.get())) + 1;
        y->height = std::max(height(y->left.get()), height(y->right.get())) + 1;

        return y;
    }

    AVLNode<Key, Value>* findMin(AVLNode<Key, Value>* node) const {
        while (node->left) {
            node = node->left.get();
        }
        return node;
    }

    std::unique_ptr<AVLNode<Key, Value>> insert(std::unique_ptr<AVLNode<Key, Value>> node, Key key, Value value) {
        if (!node) return std::make_unique<AVLNode<Key, Value>>(key, value);

        if (key < node->key)
            node->left = insert(std::move(node->left), key, value);
        else if (key > node->key)
            node->right = insert(std::move(node->right), key, value);
        else {
            node->value = value; // Обновление значения при существующем ключе
            return node;
        }

        node->height = 1 + std::max(height(node->left.get()), height(node->right.get()));
        int balance = balanceFactor(node.get());

        if (balance > 1 && key < node->left->key)
            return rightRotate(std::move(node));

        if (balance < -1 && key > node->right->key)
            return leftRotate(std::move(node));

        if (balance > 1 && key > node->left->key) {
            node->left = leftRotate(std::move(node->left));
            return rightRotate(std::move(node));
        }

        if (balance < -1 && key < node->right->key) {
            node->right = rightRotate(std::move(node->right));
            return leftRotate(std::move(node));
        }

        return node;
    }

    std::unique_ptr<AVLNode<Key, Value>> erase(std::unique_ptr<AVLNode<Key, Value>> node, Key key) {
        if (!node) return nullptr;

        if (key < node->key) {
            node->left = erase(std::move(node->left), key);
        } else if (key > node->key) {
            node->right = erase(std::move(node->right), key);
        } else {
            // Case 1: Node with only one child or no child
            if (!node->left) {
                return std::move(node->right);
            } else if (!node->right) {
                return std::move(node->left);
            }

            // Case 2: Node with two children
            AVLNode<Key, Value>* minNode = findMin(node->right.get());
            node->key = minNode->key;
            node->value = minNode->value;
            node->right = erase(std::move(node->right), minNode->key);
        }

        node->height = 1 + std::max(height(node->left.get()), height(node->right.get()));

        int balance = balanceFactor(node.get());

        if (balance > 1 && balanceFactor(node->left.get()) >= 0)
            return rightRotate(std::move(node));

        if (balance > 1 && balanceFactor(node->left.get()) < 0) {
            node->left = leftRotate(std::move(node->left));
            return rightRotate(std::move(node));
        }

        if (balance < -1 && balanceFactor(node->right.get()) <= 0)
            return leftRotate(std::move(node));

        if (balance < -1 && balanceFactor(node->right.get()) > 0) {
            node->right = rightRotate(std::move(node->right));
            return leftRotate(std::move(node));
        }

        return node;
    }

    AVLNode<Key, Value>* search(AVLNode<Key, Value>* node, Key key) const {
        if (!node || node->key == key) return node;

        if (key < node->key)
            return search(node->left.get(), key);
        else
            return search(node->right.get(), key);
    }

    void inorder(const AVLNode<Key, Value>* node, std::vector<std::pair<Key, Value>>& result) const {
        if (node) {
            inorder(node->left.get(), result);
            result.push_back({node->key, node->value});
            inorder(node->right.get(), result);
        }
    }

public:
    void insert(Key key, Value value) {
        root = insert(std::move(root), key, value);
    }

    void erase(Key key) {
        root = erase(std::move(root), key);
    }

    Value* search(Key key) {
        auto node = search(root.get(), key);
        return node ? &node->value : nullptr;
    }

    std::vector<std::pair<Key, Value>> getAllNodes() const {
        std::vector<std::pair<Key, Value>> result;
        inorder(root.get(), result);
        return result;
    }

    void printInorder() const {
        std::vector<std::pair<Key, Value>> nodes = getAllNodes();
        for (const auto& node : nodes) {
            cout << node.first << " ";
        }
        cout << endl;
    }
};

AVLTree<int, NodeInfo> tree;
std::atomic<bool> stop_heartbit(false);

bool isProcessAlive(int pid, int id) {
    int status;
    pid_t result = waitpid(pid, &status, WNOHANG); // Проверяем состояние процесса

    if (result == 0) {
        // Процесс живой, но не завершен
        return kill(pid, 0) == 0;
    } else if (result == pid) {
        // Процесс завершился, удаляем его из системы
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            tree.erase(id);
            cout << "Process " << pid << " became a zombie and was reaped." << endl;
            return false;
        }
    }

    // Ошибка или процесс больше не существует
    return false;
}

void heartbit() {
    while (!stop_heartbit) {
        std::this_thread::sleep_for(std::chrono::seconds(20));

        std::vector<std::pair<int, NodeInfo>> nodes = tree.getAllNodes();
        for (const auto& node : nodes) {
            int id = node.first;
            NodeInfo node_info = node.second;
            if (isProcessAlive(node_info.pid, id)) {
                cout << "Process[" << id << "] is alive" << endl;
            } else {
                cout << "Process[" << id << "] is unavailable" << endl;
            }
        }
    }
}

std::string createNode(int id) {
    if (tree.search(id)) {
        return "Error: Node already exists";
    }

    int pid = fork();
    if (pid == -1) {
        return "Error: Fork failed";
    }

    if (pid == 0) {
        execl("./node", "./node", std::to_string(id).c_str(), NULL);
        exit(0);
    } else {
        std::ostringstream endpoint;
        endpoint << "tcp://127.0.0.1:" << 6666 + id;

        NodeInfo node_info = {id, pid, endpoint.str()};
        tree.insert(id, node_info);
        return "Ok: Created node " + std::to_string(id) + " with PID " + std::to_string(pid);
    }
}

std::string execCommand(int id, const std::string& command) {
    NodeInfo* node_info = tree.search(id);
    if (!node_info) {
        return "Error: Node not found";
    }

    if (!isProcessAlive(node_info->pid, node_info->id)){
        return "Error: Node process not alive";
    }

    try {
        zmq::context_t context(1);
        zmq::socket_t socket(context, ZMQ_REQ);
        socket.connect(node_info->endpoint);

        zmq::message_t request(command.size());
        memcpy(request.data(), command.data(), command.size());
        socket.send(request, zmq::send_flags::none);

        zmq::message_t reply;
        zmq::recv_result_t recv_result = socket.recv(reply, zmq::recv_flags::none);
        if (!recv_result) {
            return "Error: Failed to receive reply from node";
        }

        return std::string(static_cast<char*>(reply.data()), reply.size());
    } catch (const zmq::error_t& e) {
        return "Error: Node is unavailable (" + std::string(e.what()) + ")";
    } catch (...) {
        return "Error: Unexpected failure";
    }
}

std::string pingNode(int id) {
    NodeInfo* node_info = tree.search(id);
    if (!node_info) {
        return "Error: Node not found";
    }

    if (!isProcessAlive(node_info->pid, node_info->id)){
        return "Error: Node process not alive";
    }

    try {
        zmq::context_t context(1);
        zmq::socket_t socket(context, ZMQ_REQ);

        socket.connect(node_info->endpoint);

        zmq::message_t ping("ping", 4);
        auto send_result = socket.send(ping, zmq::send_flags::none);
        if (!send_result) {
            return "Ok: 0 (send failed)";
        }

        zmq::message_t reply;
        auto recv_result = socket.recv(reply, zmq::recv_flags::none);

        if (recv_result.value()) {
            return "Ok: 1";
        } else {
            return "Ok: 0 (no reply)";
        }
    } catch (const zmq::error_t& e) {
        return "Ok: 0 (exception: " + std::string(e.what()) + ")";
    } catch (...) {
        return "Ok: 0 (unknown error)";
    }
}

int main() {
    cout << "Manager started. Commands: create id, exec id [cmd], ping id" << endl;

    std::thread heartbit_thread(heartbit);

    std::string input;
    while (getline(cin, input)) {
        std::istringstream iss(input);
        std::string cmd;
        int id;
        iss >> cmd >> id;

        if (cmd == "create") {
            cout << createNode(id) << endl;
            tree.printInorder();
        } else if (cmd == "exec") {
            std::string subcmd;
            iss >> subcmd;
            cout << execCommand(id, subcmd) << endl;
        } else if (cmd == "ping") {
            cout << pingNode(id) << endl;
        } else {
            cout << "Error: Invalid command" << endl;
        }
    }

    stop_heartbit = true;
    heartbit_thread.join();

    return 0;
}