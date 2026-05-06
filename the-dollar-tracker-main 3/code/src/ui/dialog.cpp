#include "dialog.h"
#include "ui_dialog.h"
#include "../core/FileGateway.h"
#include "../core/Paths.h"
#include <QMessageBox>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle("Add Manager");

    connect(ui->saveBtn, &QPushButton::clicked, this, [this]{
        const QString id = ui->editId->text().trimmed();
        if (id.isEmpty()) { QMessageBox::warning(this,"Add Manager","Enter ID."); return; }
        auto mg = FileGateway::instance().readAll(Paths::file("mg_ids.txt")).value_or("");
        for (const auto &L : mg.split('\n')) if (L.trimmed() == id) { QMessageBox::information(this,"Add Manager","Already a manager."); return; }
        FileGateway::instance().appendLine(Paths::file("mg_ids.txt"), id);
        accept();
    });

    connect(ui->closeBtn, &QPushButton::clicked, this, &QDialog::reject);
}
Dialog::~Dialog(){ delete ui; }
