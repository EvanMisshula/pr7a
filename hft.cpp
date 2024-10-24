#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <random>
#include <chrono>

// Shared data structures
std::queue<std::string> orderQueue;
std::atomic<double> marketPrice(100.0);  // Initial price
std::atomic<int> position(0);  // Portfolio position
std::mutex orderMutex;
std::condition_variable orderCondition;

// Order placement function
void placeOrders() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulate delay for placing orders
        std::lock_guard<std::mutex> lock(orderMutex);
        std::string order = "Buy 10 @ Market";
        orderQueue.push(order);
        std::cout << "Order placed: " << order << std::endl;
        orderCondition.notify_one();
    }
}

// Market price update function
void updateMarketPrice() {
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(-1.0, 1.0);
    
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        double change = distribution(generator);
        marketPrice += change;
        std::cout << "Market price updated to: " << marketPrice << std::endl;
    }
}

// Order execution function
void executeOrders() {
    while (true) {
        std::unique_lock<std::mutex> lock(orderMutex);
        orderCondition.wait(lock, []{ return !orderQueue.empty(); });
        std::string order = orderQueue.front();
        orderQueue.pop();
        lock.unlock();
        
        std::cout << "Executing order: " << order << " at price: " << marketPrice << std::endl;
    }
}

// Risk management function
void riskManagement() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        if (position > 100) {
            std::lock_guard<std::mutex> lock(orderMutex);
            orderQueue.push("Sell 10 @ Market to reduce risk");
            orderCondition.notify_one();
            std::cout << "Risk management triggered: Reducing position." << std::endl;
        }
    }
}

int main() {
    std::thread orderThread(placeOrders);
    std::thread priceThread(updateMarketPrice);
    std::thread execThread(executeOrders);
    std::thread riskThread(riskManagement);

    // Join threads to wait for completion (this would typically have a termination condition)
    orderThread.join();
    priceThread.join();
    execThread.join();
    riskThread.join();
    
    return 0;
}
