#include "ocd_core.hpp"

ocd_core::ocd_core(Logger *logger, QObject *parent)
    : QObject(parent)
    , m_logger(logger)
    , m_oocd_cmd(std::make_unique<oocd_cmd>(this))
    , m_state(CoreState::IDLE)
    , m_is_connected(false)
{
    // Connect telnet_client's response signal to our response handler
    connect(m_oocd_cmd->get_telnet_client(), &telnet_client::responseReady,
            this, &ocd_core::onResponseReceived);
}

void ocd_core::enqueueCommand(const Command &cmd)
{
    m_command_queue.enqueue(cmd);
    m_logger->info(QString("Command enqueued: %1").arg(static_cast<int>(cmd.type)));
    
    // Trigger processing only if core is IDLE
    // If EXECUTING or WAITING_RESPONSE, the command will be processed later
    if (m_state == CoreState::IDLE) {
        processNextCommand();
    }
}

void ocd_core::processNextCommand()
{
    // Change state to EXECUTING
    m_state = CoreState::EXECUTING;
    
    // Dequeue and store current command
    m_current_command = m_command_queue.dequeue();
    
    m_logger->info(QString("Executing command: %1").arg(static_cast<int>(m_current_command.type)));
    
    // Send command via oocd_cmd
    m_oocd_cmd->send_command(m_current_command.type, m_current_command.args);
    
    // Transition to WAITING_RESPONSE state
    // We'll wait for response from OpenOCD before processing next command
    m_state = CoreState::WAITING_RESPONSE;
}

void ocd_core::connectToOpenOCD(const QString &ip, uint32_t port)
{
    // Connection is independent of command queue
    if (m_is_connected) {
        m_logger->warning("Already connected to OpenOCD");
        return;
    }
    
    m_logger->info(QString("Connecting to OpenOCD at %1:%2").arg(ip).arg(port));
    
    // Attempt to connect via oocd_cmd
    bool connected = m_oocd_cmd->connect(ip, port);
    
    if (connected) {
        m_logger->info("Connected successfully!");
        m_is_connected = true;
        emit connectionStatusChanged(true);
    } else {
        m_logger->error("Failed to connect to OpenOCD");
        emit errorOccurred("Connection failed");
        m_is_connected = false;
        emit connectionStatusChanged(false);
    }
}

void ocd_core::disconnectFromOpenOCD()
{
    if (!m_is_connected) {
        m_logger->warning("Not connected to OpenOCD");
        return;
    }
    
    m_logger->info("Disconnecting from OpenOCD");
    
    m_oocd_cmd->disconnect();
    m_is_connected = false;
    emit connectionStatusChanged(false);
    m_logger->info("Disconnected successfully!");
}

void ocd_core::onResponseReceived(const QByteArray &response)
{
    // This slot is called when telnet_client receives response from OpenOCD
    QString response_str = QString::fromUtf8(response);
    m_logger->info(QString("Response received: %1").arg(response_str.trimmed()));
    
    // Process response based on current command
    // Here you can validate if command succeeded, parse data, etc.
        
    // Return to IDLE state
    m_state = CoreState::IDLE;
    
    // Process next command in queue if any
    if (!m_command_queue.isEmpty()) {
        processNextCommand();
    }
}
