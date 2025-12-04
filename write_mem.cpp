#include "write_mem.h"
#include "ui_write_mem.h"
#include <QDebug>

write_mem::write_mem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::write_mem)
{
    ui->setupUi(this);
}

write_mem::~write_mem()
{
    delete ui;
}


void write_mem::on_save_btn_clicked()
{
    data_to_write = ui->mem_tedit->toPlainText();
    qDebug() << "Data is saved" << data_to_write;
}

