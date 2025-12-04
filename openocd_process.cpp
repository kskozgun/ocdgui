#include "openocd_process.hpp"
#include <QDebug>
#include <QFileInfo>
#include <QDir>

openocd_process::openocd_process(const QString& binPath, const QStringList& arguments)
    : QObject(nullptr),
    openocd_bin_path(binPath),
    openocd_arguments(arguments),
    process(std::make_unique<QProcess>())
{
    // Connect process signals to slots for automatic output handling
    connect(process.get(), &QProcess::readyReadStandardOutput, this, &openocd_process::ready_read_stdout);
    connect(process.get(), &QProcess::readyReadStandardError, this, &openocd_process::ready_read_stderr);
    connect(process.get(), &QProcess::finished, this, &openocd_process::process_finished);
    connect(process.get(), &QProcess::errorOccurred, this, &openocd_process::process_error);
}

openocd_process::~openocd_process()
{
    if (process && process->state() != QProcess::NotRunning) {
        process->kill();
        process->waitForFinished();
    }
}

bool openocd_process::start()
{
    if (process->state() != QProcess::NotRunning) {
        debugConsoleError("OpenOCD process is already running");
        return false;
    }

    // Validate binary path before attempting to start
    QFileInfo fileInfo(openocd_bin_path);
    if (!fileInfo.exists()) {
        debugConsoleError("OpenOCD binary not found: " + openocd_bin_path);
        return false;
    }
    if (!fileInfo.isFile()) {
        debugConsoleError("OpenOCD path is not a file: " + openocd_bin_path);
        return false;
    }
    if (!fileInfo.isExecutable()) {
        debugConsoleError("OpenOCD binary is not executable: " + openocd_bin_path);
        return false;
    }

    debugConsoleInfo("Starting OpenOCD: " + openocd_bin_path);
    debugConsoleInfo("Arguments: " + openocd_arguments.join(" "));

    process->setProgram(openocd_bin_path);
    process->setArguments(openocd_arguments);
    process->start();

    if (!process->waitForStarted(3000)) {
        debugConsoleError("Failed to start OpenOCD: " + process->errorString());
        debugConsoleError("Working directory: " + QDir::currentPath());
        return false;
    } else {
        debugConsoleInfo("OpenOCD started successfully");
        return true;
    }
}

void openocd_process::kill()
{
    if (process->state() == QProcess::NotRunning) {
        debugConsoleError("OpenOCD process is not running");
        return;
    }

    debugConsoleInfo("Killing OpenOCD process...");
    process->kill();
    process->waitForFinished();
    debugConsoleInfo("OpenOCD killed");
}

void openocd_process::debugConsoleInfo(const QString& message)
{
    qDebug() << "[OpenOCD INFO]" << message;
}

void openocd_process::debugConsoleError(const QString& message)
{
    qDebug() << "[OpenOCD ERROR]" << message;
}

void openocd_process::ready_read_stdout()
{
    QString output = QString::fromLocal8Bit(process->readAllStandardOutput());
    if (!output.isEmpty()) {
        debugConsoleInfo(output.trimmed());
    }
}

void openocd_process::ready_read_stderr()
{
    QString error = QString::fromLocal8Bit(process->readAllStandardError());
    if (!error.isEmpty()) {
        debugConsoleError(error.trimmed());
    }
}

void openocd_process::process_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QString statusStr = (exitStatus == QProcess::NormalExit) ? "normally" : "crashed";
    debugConsoleInfo("OpenOCD process finished " + statusStr + " with exit code: " + QString::number(exitCode));
    emit processFinished(exitCode, exitStatus);
}

void openocd_process::process_error(QProcess::ProcessError error)
{
    debugConsoleError("OpenOCD process error occurred");
    emit processError();
}
