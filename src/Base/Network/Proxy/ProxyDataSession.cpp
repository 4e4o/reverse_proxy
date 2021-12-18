#include "ProxyDataSession.hpp"

ProxyDataSession::~ProxyDataSession() {
    m_otherOnWrite.disconnect();
    m_otherOnClose.disconnect();
}

void ProxyDataSession::setOther(std::shared_ptr<ProxyDataSession> ps) {
    if (m_other != nullptr)
        return;

    m_other = ps;
    m_other->setOther(std::dynamic_pointer_cast<ProxyDataSession>(shared_from_this()));
}

void ProxyDataSession::start() {
    if (m_other == nullptr)
        return;

    onData.connect([this](const uint8_t *ptr, std::size_t size) {
        writeToOther(ptr, size);
    });

    onClose.connect([this]() {
        onOurClose();
    });

    m_otherOnWrite.disconnect();
    m_otherOnClose.disconnect();

    m_otherOnWrite = m_other->onWriteDone.connect([this]() {
        onOtherSessionWriteDone();
    });

    m_otherOnClose = m_other->onClose.connect([this]() {
        onOtherSessionClosed();
    });

    readSome();
}

void ProxyDataSession::writeToOther(const uint8_t *ptr, std::size_t size) {
    if (m_other == nullptr)
        return;

    m_other->writeAll(ptr, size);
}

void ProxyDataSession::onOurClose() {
    m_otherOnWrite.disconnect();
    m_otherOnClose.disconnect();

    // когда соединение закрыто надо освободить ссылку на другую сессию
    // чтоб объект другой сессии умер после завершения своих операций
    m_other.reset();
}

void ProxyDataSession::onOtherSessionWriteDone() {
    readSome();
}

void ProxyDataSession::onOtherSessionClosed() {
    closeOnWrite();
}
