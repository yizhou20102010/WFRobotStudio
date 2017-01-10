#ifndef MYINPUTDIALOG_H
#define MYINPUTDIALOG_H

#include <QDialog>

namespace Ui {
class MyInputDialog;
}

class MyInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MyInputDialog(QWidget *parent = 0);
    ~MyInputDialog();

signals:
    void PathsNameListSelChanged(int index);


private slots:
    void on_PathsNameList_currentIndexChanged(int index);

private:
    Ui::MyInputDialog *ui;
};

#endif // MYINPUTDIALOG_H
