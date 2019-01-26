#include "setvaluedialog.h"
#include "ui_setvaluedialog.h"

SetValueDialog::SetValueDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetValueDialog)
{
    ui->setupUi(this);
    rho = 100;
    maxrho = 1000;
    minrho = 1;
    minangle = 30;
    maxlength = 1000;
    ui->lineEdit_rho->setText(QString::number(rho));
    ui->lineEdit_maxrho->setText(QString::number(maxrho));
    ui->lineEdit_minrho->setText(QString::number(minrho));
    ui->lineEdit_minangle->setText(QString::number(minangle));
    ui->lineEdit_maxlength->setText(QString::number(maxlength));
}

SetValueDialog::~SetValueDialog()
{
    delete ui;
}


void SetValueDialog::on_button_sure_clicked()
{
    rho = ui->lineEdit_rho->text().toDouble();
    maxrho = ui->lineEdit_maxrho->text().toDouble();
    minrho = ui->lineEdit_minrho->text().toDouble();
    minangle = ui->lineEdit_minangle->text().toDouble();
    maxlength = ui->lineEdit_maxlength->text().toDouble();
    emit sendValues(rho, maxrho, minrho, minangle, maxlength);
    this->close();
}

void SetValueDialog::on_button_exit_clicked()
{
    this->close();
}
