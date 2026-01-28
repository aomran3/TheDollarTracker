#include "dialog2.h"
#include "ui_dialog2.h"
#include "../core/FileGateway.h"
#include "../core/Paths.h"
#include <QMessageBox>

Dialog2::Dialog2(QWidget *parent)
    : QDialog(parent), ui(new Ui::Dialog2)
{
    ui->setupUi(this);
    setWindowTitle("Remove Manager");

    connect(ui->deleteBtn, &QPushButton::clicked, this, [this]{
        const QString id = ui->editId->text().trimmed();
        auto mgOpt = FileGateway::instance().readAll(Paths::file("mg_ids.txt"));
        if (!mgOpt) { QMessageBox::warning(this,"Remove Manager","mg_ids.txt not found."); return; }
        QStringList kept; bool removed=false;
        for (const auto &L : mgOpt->split('\n')) {
            if (L.trimmed()==id) { removed=true; continue; }
            kept << L;
        }
        if (!removed) { QMessageBox::information(this,"Remove Manager","No such manager."); return; }
        FileGateway::instance().writeAll(Paths::file("mg_ids.txt"), kept.join('\n'));
        accept();
    });

    connect(ui->closeBtn, &QPushButton::clicked, this, &QDialog::reject);
}
Dialog2::~Dialog2(){ delete ui; }
