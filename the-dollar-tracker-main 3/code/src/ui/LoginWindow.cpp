#include "LoginWindow.h"
#include "ui_LoginWindow.h"

LoginWindow::LoginWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);

    // Buttons
    connect(ui->btnOk, &QPushButton::clicked, this, [this] {
        if (!userId().isEmpty())
            accept();
    });

    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    ui->editId->setPlaceholderText("emp312 or mg3307");
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

QString LoginWindow::userId() const
{
    return ui->editId->text().trimmed();
}
