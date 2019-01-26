#include "boundarydialog.h"
#include "ui_boundarydialog.h"

BoundaryDialog::BoundaryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BoundaryDialog)
{
    ui->setupUi(this);
    top = 0;bottom = -10000;left = -10000;right = 10000;
    ui->lineEdit_top->setText(QString::number(top));
    ui->lineEdit_bottom->setText(QString::number(bottom));
    ui->lineEdit_left->setText(QString::number(left));
    ui->lineEdit_right->setText(QString::number(right));
}

BoundaryDialog::~BoundaryDialog()
{
    delete ui;
}

void BoundaryDialog::on_pushButton_sure_clicked()
{
    top = ui->lineEdit_top->text().toDouble();
    bottom = ui->lineEdit_bottom->text().toDouble();
    left = ui->lineEdit_left->text().toDouble();
    right = ui->lineEdit_right->text().toDouble();
    emit sendBoundary(top, bottom, left, right);
    this->close();
}

void BoundaryDialog::on_pushButton_Exit_clicked()
{
    this->close();
}
