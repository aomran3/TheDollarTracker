#include "addname.h"
#include "ui_addname.h"
#include "../core/FileGateway.h"
#include "../core/Paths.h"
#include <QMessageBox>

Addname::Addname(QWidget *parent)
    : QDialog(parent), ui(new Ui::Addname)
{
    ui->setupUi(this);

    // load roles
    auto rolesTxt = FileGateway::instance().readAll(Paths::file("jobs.txt")).value_or("");
    for (const auto &line : rolesTxt.split('\n'))
        if (line.trimmed().contains('$')) ui->comboRole->addItem(line.trimmed());

    connect(ui->saveBtn,  &QPushButton::clicked, this, [this]{
        const QString id = ui->editId->text().trimmed();
        const QString role = ui->comboRole->currentText().trimmed();
        if (id.isEmpty() || role.isEmpty()) { QMessageBox::warning(this,"Add Employee","Enter ID and choose a role."); return; }

        auto idsTxt = FileGateway::instance().readAll(Paths::file("ids.txt")).value_or("");
        for (const auto &L : idsTxt.split('\n'))
            if (L.startsWith(id + ",")) { QMessageBox::warning(this,"Add Employee","Employee already exists."); return; }

        const QString line = id + ", " + role;
        FileGateway::instance().appendLine(Paths::file("ids.txt"), line);
        accept();
    });

    connect(ui->closeBtn, &QPushButton::clicked, this, &QDialog::reject);
}

Addname::~Addname(){ delete ui; }
