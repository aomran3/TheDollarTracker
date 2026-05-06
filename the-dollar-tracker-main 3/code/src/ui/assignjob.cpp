#include "assignjob.h"
#include "ui_assignjob.h"

Assignjob::Assignjob(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Assignjob)
{
    ui->setupUi(this);

    connect(ui->closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

Assignjob::~Assignjob()
{
    delete ui;
}
