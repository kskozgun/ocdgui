#ifndef WRITE_MEM_H
#define WRITE_MEM_H

#include <QWidget>

namespace Ui {
class write_mem;
}

class write_mem : public QWidget
{
    Q_OBJECT

public:
    explicit write_mem(QWidget *parent = nullptr);
    ~write_mem();
    
    QString getDataToWrite() const { return data_to_write; }

private slots:
    void on_save_btn_clicked();

private:
    Ui::write_mem *ui;
    QString data_to_write;
};

#endif // WRITE_MEM_H
