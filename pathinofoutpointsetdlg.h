#ifndef PATHINOFOUTPOINTSETDLG_H
#define PATHINOFOUTPOINTSETDLG_H

#include <QDialog>

namespace Ui {
class PathInofOutPointSetDlg;
}

class PathInofOutPointSetDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PathInofOutPointSetDlg(QWidget *parent = 0);
    ~PathInofOutPointSetDlg();
    void SetDefaultName(int mode=0);
private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();
signals:
    void acceptstate(int apmode, bool isall);

private:
    Ui::PathInofOutPointSetDlg *ui;
    int addpointmode;//0,InPoint, 1,OutPoint;
};

#endif // PATHINOFOUTPOINTSETDLG_H
