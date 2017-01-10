#ifndef PATHPOSTURESETDIALOG_H
#define PATHPOSTURESETDIALOG_H

#include <QDialog>

namespace Ui {
class PathPostureSetDialog;
}

class PathPostureSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PathPostureSetDialog(QWidget *parent = 0);
    ~PathPostureSetDialog();

private slots:


    void on_Value_A_valueChanged(double arg1);

    void on_Value_B_valueChanged(double arg1);

    void on_Value_C_valueChanged(double arg1);

    void on_Confirm_clicked();

    void on_Cancle_clicked();

private:
    Ui::PathPostureSetDialog *ui;

private:
    double Euler[3];


public:
    void SetPosture(double euler[3]);//设置姿态;
    void Getposture(double euler[3]);//获取姿态
};

#endif // PATHPOSTURESETDIALOG_H
