#ifndef OOCD_CMD_HPP
#define OOCD_CMD_HPP

#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>
#include "telnet_client.hpp"
#include "command_type.hpp"

class oocd_cmd : public QObject
{
    Q_OBJECT
public:
    explicit oocd_cmd(QObject *parent = nullptr);
    
    bool connect(const QString& ip, uint32_t port);
    void disconnect();
    telnet_client* get_telnet_client() { return m_telnet_client.get(); }
    void send_command(CommandType cmd, const QStringList& args = QStringList());

private:
    std::unique_ptr<telnet_client> m_telnet_client;
    QString build_command_string(CommandType cmd, const QStringList& args);
    
signals:
};

#endif // OOCD_CMD_HPP
