#ifndef OPENOCD_PROCESS_H
#define OPENOCD_PROCESS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <memory>

class openocd_process : public QObject
{
    Q_OBJECT
public:
    explicit openocd_process(const QString& binPath, const QStringList& arguments);
    ~openocd_process();

    bool start();
    void kill();

signals:
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processError();

private:
    void debugConsoleError(const QString& message);
    void debugConsoleInfo(const QString& message);

private slots:
    void ready_read_stdout();
    void ready_read_stderr();
    void process_finished(int exitCode, QProcess::ExitStatus exitStatus);
    void process_error(QProcess::ProcessError error);

private:
    QString openocd_bin_path;
    QStringList openocd_arguments;
    std::unique_ptr<QProcess> process;
};

#endif // OPENOCD_PROCESS_H
