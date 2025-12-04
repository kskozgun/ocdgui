#ifndef OCD_CORE_HPP
#define OCD_CORE_HPP

#include <QObject>
#include <QQueue>
#include <memory>
#include "command_type.hpp"
#include "logger.h"
#include "oocd_cmd.hpp"

enum class CoreState {
    IDLE,           // No command being processed
    EXECUTING,      // Currently executing a command (sending to OpenOCD)
    WAITING_RESPONSE // Waiting for OpenOCD response before processing next
};

class ocd_core : public QObject
{
    Q_OBJECT

public:
    explicit ocd_core(Logger *logger, QObject *parent = nullptr);
    
    // Public interface for MainWindow
    void enqueueCommand(const Command &cmd);

public slots:
    // Connection management (separate from command queue)
    void connectToOpenOCD(const QString &ip, uint32_t port);
    void disconnectFromOpenOCD();

signals:
    void connectionStatusChanged(bool connected);
    void errorOccurred(const QString &error);

private slots:
    void processNextCommand();
    void onResponseReceived(const QByteArray &response);

private:
    // Members
    QQueue<Command> m_command_queue;
    Logger *m_logger;
    std::unique_ptr<oocd_cmd> m_oocd_cmd;
    CoreState m_state;
    bool m_is_connected;
    Command m_current_command;  // Track currently executing command (assigned when dequeued)
};

#endif // OCD_CORE_HPP
