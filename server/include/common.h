#pragma once

#include <iostream>
#include <sstream>
#include <string.h>
#include <string>
#include <thread>
#include <zmq.hpp>
#include <zmq.h>
#include <unistd.h>

#define XPUB_ENDPOINT "tcp://localhost:9200"
#define XSUB_ENDPOINT "tcp://localhost:9210"

const int TOPIC_LENGTH = 3;

std::string RECEIVE_TOPIC = std::string("123", TOPIC_LENGTH);
std::string RESPONSE_TOPIC = std::string("234", TOPIC_LENGTH);
std::string WELCOME_TOPIC = std::string("345", TOPIC_LENGTH);
std::string TEST_TOPIC = std::string("678", TOPIC_LENGTH);
std::string TEST_TOPIC2 = std::string("789", TOPIC_LENGTH);

typedef struct
{
    std::string topic;
    std::vector<std::string> msgs;
    int msg_count;
} multipart_msg_t;

inline void recv_multipart_msg(zmq::socket_t *socket, multipart_msg_t *msg)
{
    zmq::message_t curr_msg;

    // receive topic msg
    socket->recv(&curr_msg);
    msg->topic.assign(static_cast<char *>(curr_msg.data()), curr_msg.size());

    int recvMore = 1;
    size_t int_size = sizeof(int);
    socket->getsockopt(ZMQ_RCVMORE, &recvMore, &int_size);
    while (recvMore) {
        // need to rebuild curr msg to allow to be reused
        curr_msg.rebuild();

        socket->recv(&curr_msg);
        std::string msg_txt;
        msg_txt.assign(static_cast<char *>(curr_msg.data()), curr_msg.size());

        msg->msgs.push_back(msg_txt);
        msg->msg_count++;

        socket->getsockopt(ZMQ_RCVMORE, &recvMore, &int_size);
    }
}

inline void send_multipart_msg(zmq::socket_t *socket, multipart_msg_t *msg)
{
    zmq::message_t curr_msg(msg->topic.c_str(), msg->topic.length());

    for (auto it : msg->msgs)
    {
        socket->send(curr_msg, zmq::send_flags::sndmore);
        // copies data across
        curr_msg.rebuild(it.c_str(), it.length());
    }
    socket->send(curr_msg);
}
