#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "command_type.hpp"
#include "write_mem.h"
#include "logger.h"
#include "ocd_core.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void connectRequested(const QString &ip, uint32_t port);
    void disconnectRequested();

private slots:
    void on_reset_button_clicked();
    void on_readmem_button_clicked();
    void on_flashim_button_clicked();
    void on_wrtmem_button_clicked();
    void on_cstm_cmd_btn_clicked();
    void on_data_to_wrt_btn_clicked();
    void on_connect_btn_clicked();
    void on_select_elf_btn_clicked();

private:
    void get_openocd_bin_path(void);

private:
    Ui::MainWindow *ui;
    Logger *logger;
    ocd_core *m_ocd_core;
    QString elf_file_path;
    QString openocd_bin_path;
    bool connection_status;
    write_mem *mem_dialog;
};
#endif // MAINWINDOW_H
