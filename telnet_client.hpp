#ifndef TELNET_CLIENT_H
#define TELNET_CLIENT_H

#include <QObject>
#include <QTCPSocket>
#include <QByteArray>
#include <memory>

class telnet_client: public QObject
{
    Q_OBJECT

public:
    telnet_client();
    ~telnet_client();
    bool connect_to_host(QString ip_addr, uint32_t port);
    void disconnect_from_host(void);
    qint64 transmit_data(QByteArray data);

private slots:
    void on_ready_read();

private:
    std::unique_ptr<QTcpSocket> m_socket;
    QByteArray m_buffer;

    void process_response();

signals:
    void responseReady(QByteArray buffer);
};

#endif // TELNET_CLIENT_H
