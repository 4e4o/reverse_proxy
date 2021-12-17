#include "ThreadPool.hpp"

ThreadPool::ThreadPool(int threadNum)
    : m_work(new boost::asio::io_service::work(m_service)),
      m_threadNum(threadNum) {
}

boost::asio::io_service &ThreadPool::getIOService() {
    return m_service;
}

int ThreadPool::run() {
    for (int i = 0; i < m_threadNum; ++i)
        m_threads.emplace_back([this] () { m_service.run(); });

    m_service.run();
    return 0;
}

void ThreadPool::stop(bool join) {
    m_work.reset();

    if (join) {
        for (auto &t: m_threads)
            t.join();
    }
}
