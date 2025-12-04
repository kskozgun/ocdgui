#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QDateTime>
#include <QMessageBox>
#include <QFile>
#include <QProcess>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connection_status = false;
    mem_dialog = new write_mem();
    
    // Setup logger
    logger = new Logger(ui->terminal_text, this);
    logger->info("Welcome to OpenGUIOCD");
    
    // Setup core processor
    m_ocd_core = new ocd_core(logger, this);
    
    // Connect core signals to UI updates
    connect(m_ocd_core, &ocd_core::connectionStatusChanged, this, [this](bool connected) {
        connection_status = connected;
        ui->connect_btn->setText(connected ? "Disconnect" : "Connect");
    });
    connect(m_ocd_core, &ocd_core::errorOccurred, this, [this](const QString &error) {
        logger->error(error);
    });
    
    // Connect MainWindow signals to core slots
    connect(this, &MainWindow::connectRequested, m_ocd_core, &ocd_core::connectToOpenOCD);
    connect(this, &MainWindow::disconnectRequested, m_ocd_core, &ocd_core::disconnectFromOpenOCD);
    connect(ui->action_openocd_path, &QAction::triggered, this, &MainWindow::get_openocd_bin_path);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::get_openocd_bin_path()
{
    openocd_bin_path = QFileDialog::getExistingDirectory(this, "Get AnyFile");
}

void MainWindow::on_reset_button_clicked()
{
    CommandType cmd = CommandType::RESET_RUN;
    QAbstractButton *checkedButton = ui->rst_btngroup->checkedButton();

    // Check run or halt
    if(checkedButton->text() == "Halt") {
        cmd = CommandType::RESET_HALT;
    }

    this->m_ocd_core->enqueueCommand(Command(cmd, {}));
}


void MainWindow::on_readmem_button_clicked()
{
    QString mem_addr = ui->rmem_ledit->text();
    QString addr_size = ui->rsize_ledit->text();

    // Handle Hex Format
    QAbstractButton *checkedButton = ui->rdm_bgroup->checkedButton();
    if(checkedButton->text() == "Hex")
    {
        mem_addr = "0x" + mem_addr;
    }

    this->m_ocd_core->enqueueCommand(Command(CommandType::READ_MEMORY, {mem_addr, addr_size}));
}

void MainWindow::on_wrtmem_button_clicked()
{
    QString mem_addr = ui->wmem_ledit->text();
    QString data = mem_dialog->getDataToWrite();
    
    if (mem_addr.isEmpty()) {
        logger->error("Memory address is empty");
        return;
    }
    
    if (data.isEmpty()) {
        logger->error("No data to write. Please enter data first.");
        return;
    }
    
    // Handle Hex Format
    QAbstractButton *checkedButton = ui->wrm_btngroup->checkedButton();
    if(checkedButton && checkedButton->text() == "Hex") {
        mem_addr = "0x" + mem_addr;
    }
    
    // Split data into individual values (space-separated)
    QStringList values = data.split(' ', Qt::SkipEmptyParts);
    
    if (values.isEmpty()) {
        logger->error("No valid data values found");
        return;
    }
    
    bool ok;
    uint32_t base_addr = mem_addr.toUInt(&ok, 0);  // Convert string to number
    if (!ok) {
        logger->error("Invalid memory address format");
        return;
    }
    
    // Enqueue write command for each value with incrementing address
    for (int i = 0; i < values.size(); ++i) {
        QString addr = QString("0x%1").arg(base_addr + (i * 4), 0, 16);  // Increment by 4 bytes (word size)
        this->m_ocd_core->enqueueCommand(Command(CommandType::WRITE_MEMORY, {addr, values[i]}));
    }
    
    logger->info(QString("Enqueued %1 write commands starting at %2").arg(values.size()).arg(mem_addr));
}


void MainWindow::on_flashim_button_clicked()
{
    if(elf_file_path.isEmpty())
    {
        // Show a warning dialog
        QMessageBox::warning(this, tr("Warning"), tr("No ELF file selected"));
        return;
    }

    this->m_ocd_core->enqueueCommand(Command(CommandType::LOAD_IMAGE, {elf_file_path}));
}


void MainWindow::on_cstm_cmd_btn_clicked()
{
    QString cstm_cmd = ui->cstm_cmd_ledit->text();
    QString arguments = ui->custom_arg_ledit->text();

    QStringList args(cstm_cmd);
    args += arguments.split(' ', Qt::SkipEmptyParts);

    this->m_ocd_core->enqueueCommand(Command(CommandType::CUSTOM, args));
}


void MainWindow::on_data_to_wrt_btn_clicked()
{
    mem_dialog->show();
}


void MainWindow::on_connect_btn_clicked()
{
    if (connection_status) {
        // Emit disconnect signal
        emit disconnectRequested();
    } else {
        // Get connection parameters and emit connect signal
        QString ip = ui->ip_line_edit->text();
        if (ip.isEmpty()) {
            ip = "127.0.0.1";
        }
        QString port = ui->port_line_edit->text();
        if (port.isEmpty()) {
            port = "4444";
        }

        bool ok = false;
        uint32_t port_num = port.toUInt(&ok);
        if (!ok) {
            logger->error("Invalid port number");
            return;
        }

        emit connectRequested(ip, port_num);
    }
}


void MainWindow::on_select_elf_btn_clicked()
{
    elf_file_path = QFileDialog::getOpenFileName(this, tr("Select Elf File"), "./", "Elf File (*.elf)");
    ui->image_path_ledit->setText(elf_file_path);
}

