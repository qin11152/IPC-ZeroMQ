#pragma once
#include <mutex>
#include <condition_variable>
#include <QObject>
#include <queue>
#include "common.h"
#include <boost/serialization/singleton.hpp>

class IPCDelegate : public QObject, public boost::serialization::singleton<IPCDelegate>, public boost::noncopyable
{
    Q_OBJECT

public:
    IPCDelegate();
    ~IPCDelegate();

    bool Init();

    void UnInit();

    void sendMsg(const std::string &topic, const std::string &msg);

private:
    zmq::socket_t* m_publisher{nullptr};
    zmq::socket_t* m_subscriber{nullptr};
    std::thread m_recvThread;
    std::thread m_sendThread;
    bool m_bRunning{false};
    std::mutex m_mutex;
    std::condition_variable m_condition;
    zmq::context_t* m_context{nullptr};
    std::vector<zmq::pollitem_t> m_vecpoll;
    std::queue<multipart_msg_t> m_vecNeedSendMsg;
};