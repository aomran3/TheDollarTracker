#include "viewpaystub.h"
#include "ui_viewpaystub.h"

Viewpaystub::Viewpaystub(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Viewpaystub)
{
    ui->setupUi(this);

    // Close button closes the dialog
    connect(ui->closeBtn, &QPushButton::clicked,
            this, &QDialog::accept);
}

Viewpaystub::~Viewpaystub()
{
    delete ui;
}
