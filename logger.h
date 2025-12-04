#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTextEdit>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};

class Logger : public QObject
{
    Q_OBJECT

public:
    explicit Logger(QTextEdit *output, QObject *parent = nullptr);
    
    void info(const QString &message);
    void warning(const QString &message);
    void error(const QString &message);
    
    void log(LogLevel level, const QString &message);
    
    void setShowTimestamp(bool show);
    void setShowLevel(bool show);

private:
    QTextEdit *m_output;
    bool m_show_timestamp;
    bool m_show_level;
    
    QString formatMessage(LogLevel level, const QString &message);
    QString levelToString(LogLevel level);
    QString levelToColor(LogLevel level);

signals:
    void logMessage(LogLevel level, QString message);
};

#endif // LOGGER_H
