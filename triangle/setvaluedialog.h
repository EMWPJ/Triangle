#ifndef SETVALUEDIALOG_H
#define SETVALUEDIALOG_H

#include <QDialog>

namespace Ui {
class SetValueDialog;
}

class SetValueDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetValueDialog(QWidget *parent = 0);
    ~SetValueDialog();

private slots:
    void on_button_sure_clicked();
    void on_button_exit_clicked();

signals:
    void sendValues(double rho, double maxrho, double minrho, double minangle, double maxlength);


private:
    Ui::SetValueDialog *ui;

    double rho, maxrho, minrho, minangle, maxlength;
};

#endif // SETVALUEDIALOG_H
