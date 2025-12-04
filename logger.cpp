#include "logger.h"

Logger::Logger(QTextEdit *output, QObject *parent)
    : QObject(parent)
    , m_output(output)
    , m_show_timestamp(true)
    , m_show_level(true)
{
    if (m_output) {
        m_output->setReadOnly(true);
        m_output->document()->setMaximumBlockCount(1000); // Limit to 1000 lines
    }
}

void Logger::info(const QString &message)
{
    log(LogLevel::INFO, message);
}

void Logger::warning(const QString &message)
{
    log(LogLevel::WARNING, message);
}

void Logger::error(const QString &message)
{
    log(LogLevel::ERROR, message);
}

void Logger::log(LogLevel level, const QString &message)
{
    QString formatted = formatMessage(level, message);
    
    if (m_output) {
        // Color support using HTML
        QString colorCode = levelToColor(level);
        
        // Escape HTML first, then convert newlines to <br>
        QString processedMessage = formatted.toHtmlEscaped();
        processedMessage.replace("\r\n", "<br>");
        processedMessage.replace("\n", "<br>");
        
        QString htmlMessage = QString("<span style='color:%1;'>%2</span>").arg(colorCode, processedMessage);
        m_output->append(htmlMessage);
    }
    
    emit logMessage(level, message);
}

void Logger::setShowTimestamp(bool show)
{
    m_show_timestamp = show;
}

void Logger::setShowLevel(bool show)
{
    m_show_level = show;
}

QString Logger::formatMessage(LogLevel level, const QString &message)
{
    QString result;
    
    if (m_show_timestamp) {
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        result += "[" + timestamp + "] ";
    }
    
    if (m_show_level) {
        result += "[" + levelToString(level) + "] ";
    }
    
    result += message;
    return result;
}

QString Logger::levelToString(LogLevel level)
{
    switch (level) {
        case LogLevel::INFO:      return "INFO";
        case LogLevel::WARNING:   return "WARN";
        case LogLevel::ERROR:     return "ERROR";
        default:                  return "LOG";
    }
}

QString Logger::levelToColor(LogLevel level)
{
    switch (level) {
        case LogLevel::INFO:      return "#0066CC"; // Blue
        case LogLevel::WARNING:   return "#FF9900"; // Orange
        case LogLevel::ERROR:     return "#CC0000"; // Red
        default:                  return "#000000"; // Black
    }
}
