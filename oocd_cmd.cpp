#include "oocd_cmd.hpp"
#include <QDebug>
#include <QMap>

// Command string mapping - maps CommandType to OpenOCD command strings
static const QMap<CommandType, QString> command_map = {
    // Target Control
    {CommandType::RESET_HALT, "reset halt"},
    {CommandType::RESET_RUN, "reset run"},
    {CommandType::HALT, "halt"},
    {CommandType::RESUME, "resume"},
    {CommandType::STEP, "step"},
    
    // Memory Operations
    {CommandType::READ_MEMORY, "mdw"},      // Read word (32-bit)
    {CommandType::WRITE_MEMORY, "mww"},     // Write word (32-bit)
    {CommandType::FILL_MEMORY, "mww"},      // Fill memory (reuse mww with loop)
    
    // Flash Operations
    {CommandType::LOAD_IMAGE, "load_image"},
    
    // Debug
    {CommandType::READ_REGISTER, "reg"},
    {CommandType::WRITE_REGISTER, "reg"},
    {CommandType::BREAKPOINT_SET, "bp"},
    {CommandType::BREAKPOINT_CLEAR, "rbp"},
    
    // System
    {CommandType::VERSION, "version"},
    {CommandType::SHUTDOWN, "shutdown"},
    {CommandType::CUSTOM, ""}
};

oocd_cmd::oocd_cmd(QObject *parent)
    : QObject{parent}
    , m_telnet_client(std::make_unique<telnet_client>())
{}

bool oocd_cmd::connect(const QString& ip, uint32_t port)
{
    if (!m_telnet_client) {
        qDebug() << "Error: telnet_client not available";
        return false;
    }
    
    return m_telnet_client->connect_to_host(ip, port);
}

void oocd_cmd::disconnect()
{
    if (!m_telnet_client) {
        qDebug() << "Error: telnet_client not available";
        return;
    }
    
    m_telnet_client->disconnect_from_host();
}

void oocd_cmd::send_command(CommandType cmd, const QStringList& args)
{
    if (!m_telnet_client) {
        qDebug() << "Error: telnet_client not available";
        return;
    }
    
    QString command_str = build_command_string(cmd, args);
    QByteArray command_data = command_str.toUtf8();
    
    qDebug() << "Sending command:" << command_str.trimmed();
    m_telnet_client->transmit_data(command_data);
}

QString oocd_cmd::build_command_string(CommandType cmd, const QStringList& args)
{
    QString command;
    
    if (cmd == CommandType::CUSTOM) {
        // For CUSTOM commands, args[0] is the command and the rest are its arguments
        command = args.join(" ");
    } else {
        command = command_map.value(cmd, "");
        if (!args.isEmpty()) {
            command += " " + args.join(" ");
        }
    }
    
    // Add line terminator if not present
    if (!command.endsWith("\r\n")) {
        command += "\r\n";
    }
    
    return command;
}
