//
// Created by chashika on 2020-07-08.
//

#include "wso2MB_client.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <amqpcpp.h>
#include "conn_handler.h"

int
main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: <bind key>..." << std::endl;
        return 0;
    }
    ConnHandler handler;
    AMQP::TcpConnection connection(handler,
                                   AMQP::Address("localhost", 5672,
                                                 AMQP::Login("admin", "admin"), "/"));
    AMQP::TcpChannel channel(&connection);
    channel.onError([&handler](const char *message) {
        std::cout << "Channel error: " << message << std::endl;
        handler.Stop();
    });
    channel.declareExchange("amq.topic", AMQP::topic);
    channel.declareQueue("queu3", AMQP::exclusive);
    std::for_each(argv + 1, argv + argc,
                  [&](const char *bind_key) {
                      std::cout << bind_key << std::endl;
                      channel.bindQueue("amq.topic", "queu3", "throttleData");
                      channel.consume("queu3", AMQP::noack)
                              .onReceived
                                      (
                                              [](const AMQP::Message &m, uint64_t, bool) {
                                                  std::cout << m.routingKey() << " this is ";
                                                  std::cout << m.routingKey() << ": "
                                                            << m.message() << std::endl;
                                              }
                                      );
                  }
    );
    handler.Start();
    std::cout << "Closing connection." << std::endl;
    connection.close();
    return 0;
}


