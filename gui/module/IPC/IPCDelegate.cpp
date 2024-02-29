#include "IPCDelegate.h"

const int TOPIC_LENGTH = 3;

std::string CLOSE_TOPIC = std::string("000", TOPIC_LENGTH);
std::string RECEIVE_TOPIC = std::string("123", TOPIC_LENGTH);
std::string RESPONSE_TOPIC = std::string("234", TOPIC_LENGTH);
std::string WELCOME_TOPIC = std::string("345", TOPIC_LENGTH);

IPCDelegate::IPCDelegate()
{
}

IPCDelegate::~IPCDelegate()
{
  if (m_publisher)
  {
    m_publisher->close();
    delete m_publisher;
    m_publisher = nullptr;
  }
  if (m_subscriber)
  {
    m_subscriber->close();
    delete m_subscriber;
    m_subscriber = nullptr;
  }
  if (m_context)
  {
    m_context->close();
    delete m_context;
    m_context = nullptr;
  }
}

bool IPCDelegate::Init()
{
  m_context = new zmq::context_t(2);
  m_publisher = new zmq::socket_t(*m_context, ZMQ_PUB);
  m_subscriber = new zmq::socket_t(*m_context, ZMQ_SUB);

  m_bRunning = true;
  std::string pub_transport(XSUB_ENDPOINT);
  try
  {
    // The port number here is the XSUB port of the Msg Proxy service (9200)
    m_publisher->connect(pub_transport);
  }
  catch (zmq::error_t e)
  {
    std::cerr << "Error connection to " << pub_transport << ". Error is: " << e.what() << std::endl;
    return false;
  }
  std::string sub_transport(XPUB_ENDPOINT);
  try
  {
    // The subscriber socket
    // The port number here is the XSUB port of the Msg Proxy service (9210)
    m_subscriber->setsockopt(ZMQ_SUBSCRIBE, WELCOME_TOPIC.c_str(), WELCOME_TOPIC.length());
    m_subscriber->setsockopt(ZMQ_SUBSCRIBE, RECEIVE_TOPIC.c_str(), RECEIVE_TOPIC.length());
    m_subscriber->setsockopt(ZMQ_SUBSCRIBE, CLOSE_TOPIC.c_str(), CLOSE_TOPIC.length());
    m_subscriber->connect(sub_transport);

    // helps with slow connectors!
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  catch (zmq::error_t e)
  {
    std::cerr << "Error connection to " << sub_transport << ". Error is: " << e.what() << std::endl;
    return false;
  }

  m_vecpoll.push_back({*m_subscriber, 0, ZMQ_POLLIN, 0});

  std::thread tmpThread([=]()
                        {
    size_t int_size = sizeof(int);

    while (m_bRunning)
    {
      zmq::poll(m_vecpoll.data(), 1, -1);
      if (m_vecpoll[0].revents & ZMQ_POLLIN)
      {
        multipart_msg_t msg;

        recv_multipart_msg(m_subscriber, &msg);
        std::cout << "[SUBSCRIBER]: Received Topic:'" << msg.topic << std::endl;

        if (msg.topic == WELCOME_TOPIC) 
        {
          std::cout << "[SUBSCRIBER]: Welcome message recved. Okay to do stuff" << std::endl;
          continue;
        }

        for (auto m : msg.msgs) 
        {
          std::cout << "[SUBSCRIBER]: Received '" << m << "' on topic" << msg.topic << std::endl;
          //you can do what you want about this topic.like
          // if("xxxx"==msg.topic)
          // {
          //   emit signal;
          // }
        }
      }
    } });
  m_recvThread = std::move(tmpThread);

  std::thread tmpSendThread([=]()
                            {
    while (m_bRunning) {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_condition.wait(lock, [this] { return !m_vecNeedSendMsg.empty() || !m_bRunning; });
            while (!m_vecNeedSendMsg.empty()) {
                auto msg = m_vecNeedSendMsg.front();
                m_vecNeedSendMsg.pop();
                send_multipart_msg(m_publisher, &msg);
            }
        } });

  m_sendThread=std::move(tmpSendThread);

  return true;
}

void IPCDelegate::UnInit()
{
  m_vecpoll.clear();
  m_bRunning = false;
  sendMsg(CLOSE_TOPIC, "empty");
  m_recvThread.join();
  m_sendThread.join();
}

void IPCDelegate::sendMsg(const std::string &topic, const std::string &msg)
{
  multipart_msg_t ack_msg;
  ack_msg.topic = topic;
  ack_msg.msgs.push_back(msg);

  std::unique_lock<std::mutex> lock(m_mutex);
  m_vecNeedSendMsg.push(ack_msg);
  m_condition.notify_one();
}
