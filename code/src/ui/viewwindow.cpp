#include "viewwindow.h"
#include "ui_viewwindow.h"

Viewwindow::Viewwindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Viewwindow)
{
    ui->setupUi(this);

    // Close button closes the window
    connect(ui->closeBtn, &QPushButton::clicked,
            this, &QDialog::accept);
}

Viewwindow::~Viewwindow()
{
    delete ui;
}
