#include "jobsview.h"
#include "ui_jobsview.h"

#include <QPushButton>

Jobsview::Jobsview(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Jobsview)
{
    ui->setupUi(this);
    if (auto btn = ui->closeBtn) {
        connect(btn, &QPushButton::clicked, this, &QDialog::accept);
    }
}

Jobsview::~Jobsview()
{
    delete ui;
}
