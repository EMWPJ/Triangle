#ifndef BOUNDARYDIALOG_H
#define BOUNDARYDIALOG_H

#include <QDialog>

namespace Ui {
class BoundaryDialog;
}

class BoundaryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BoundaryDialog(QWidget *parent = 0);
    ~BoundaryDialog();

private slots:
    void on_pushButton_sure_clicked();

    void on_pushButton_Exit_clicked();

signals:
    void sendBoundary(double a, double b, double c, double d);

private:
    Ui::BoundaryDialog *ui;
    double top;
    double bottom;
    double left;
    double right;


};

#endif // BOUNDARYDIALOG_H
