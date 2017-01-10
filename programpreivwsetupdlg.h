#ifndef PROGRAMPREIVWSETUPDLG_H
#define PROGRAMPREIVWSETUPDLG_H

#include <QDialog>

namespace Ui {
class ProgramPreivwSetupDlg;
}

class ProgramPreivwSetupDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ProgramPreivwSetupDlg(QWidget *parent = 0);
    ~ProgramPreivwSetupDlg();

private slots:
    void on_GenerateProgramPreview_clicked();

private:
    Ui::ProgramPreivwSetupDlg *ui;
};

#endif // PROGRAMPREIVWSETUPDLG_H
