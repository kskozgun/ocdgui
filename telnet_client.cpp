#include "telnet_client.hpp"
#include <QDebug>

telnet_client::telnet_client()
{
    m_socket = std::make_unique<QTcpSocket>();
}

telnet_client::~telnet_client()
{
    // Smart pointer handles cleanup automatically
}

bool telnet_client::connect_to_host(QString ip_addr, uint32_t port)
{
    m_socket->connectToHost(ip_addr, port);
    if(m_socket->waitForConnected(5000)) {
        connect(m_socket.get(), &QTcpSocket::readyRead, this, &telnet_client::on_ready_read);
        return true;
    }
    else {
        return false;
    }
}

void telnet_client::disconnect_from_host(void)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        disconnect(m_socket.get(), &QTcpSocket::readyRead, this, &telnet_client::on_ready_read);
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            m_socket->waitForDisconnected(3000);
        }
    }
}

qint64 telnet_client::transmit_data(QByteArray data)
{
    qint64 bytes_written = m_socket->write(data);
    m_socket->flush();
    return bytes_written;
}

void telnet_client::on_ready_read()
{
    while (m_socket->bytesAvailable()) {
        QByteArray data = m_socket->readAll();
        m_buffer.append(data);
        qDebug() << "Telnet received: " << data;
    }
    
    process_response();
}

void telnet_client::process_response()
{
    int promptIndex = m_buffer.indexOf("> ");
    if (promptIndex != -1) {
        // Extract response without the prompt
        QByteArray response = m_buffer.left(promptIndex);
        
        // Remove processed data including the prompt from buffer
        m_buffer.remove(0, promptIndex + 2);
        
        qDebug() << "Response complete: " << response;
        emit responseReady(response);
    }
}
