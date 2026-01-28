#include "jobremove.h"
#include "ui_jobremove.h"
#include "../core/FileGateway.h"
#include "../core/Paths.h"
#include <QMessageBox>

Jobremove::Jobremove(QWidget *parent)
    : QDialog(parent), ui(new Ui::Jobremove)
{
    ui->setupUi(this);
    auto rolesTxt = FileGateway::instance().readAll(Paths::file("jobs.txt")).value_or("");
    for (const auto &L : rolesTxt.split('\n')) if (L.trimmed().contains('$')) ui->comboRole->addItem(L.trimmed());

    connect(ui->deleteBtn, &QPushButton::clicked, this, [this]{
        const QString toRemove = ui->comboRole->currentText().trimmed();
        auto rolesOpt = FileGateway::instance().readAll(Paths::file("jobs.txt"));
        if (!rolesOpt) { QMessageBox::warning(this,"Remove Role","jobs.txt not found."); return; }
        QStringList kept; bool removed=false;
        for (const auto &L : rolesOpt->split('\n')) {
            if (L.trimmed()==toRemove) { removed=true; continue; }
            kept << L;
        }
        if (!removed) { QMessageBox::information(this,"Remove Role","Role not found."); return; }
        FileGateway::instance().writeAll(Paths::file("jobs.txt"), kept.join('\n'));
        accept();
    });

    connect(ui->closeBtn, &QPushButton::clicked, this, &QDialog::reject);
}
Jobremove::~Jobremove(){ delete ui; }
