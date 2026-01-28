#include "removename.h"
#include "ui_removename.h"
#include "../core/FileGateway.h"
#include "../core/Paths.h"
#include <QMessageBox>

Removename::Removename(QWidget *parent)
    : QDialog(parent), ui(new Ui::Removename)
{
    ui->setupUi(this);
    connect(ui->deleteBtn, &QPushButton::clicked, this, [this]{
        const QString id = ui->editId->text().trimmed();
        if (id.isEmpty()) { QMessageBox::warning(this,"Remove Employee","Enter an ID."); return; }

        auto idsOpt = FileGateway::instance().readAll(Paths::file("ids.txt"));
        if (!idsOpt) { QMessageBox::warning(this,"Remove Employee","ids.txt not found."); return; }

        QStringList kept;
        bool removed = false;
        for (const auto &L : idsOpt->split('\n')) {
            if (L.startsWith(id + ",")) { removed = true; continue; }
            kept << L;
        }
        if (!removed) { QMessageBox::information(this,"Remove Employee","No such employee."); return; }
        FileGateway::instance().writeAll(Paths::file("ids.txt"), kept.join('\n'));
        accept();
    });
    connect(ui->closeBtn, &QPushButton::clicked, this, &QDialog::reject);
}

Removename::~Removename(){ delete ui; }
