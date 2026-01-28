#include "jobadd.h"
#include "ui_jobadd.h"
#include "../core/FileGateway.h"
#include "../core/Paths.h"
#include <QMessageBox>

Jobadd::Jobadd(QWidget *parent)
    : QDialog(parent), ui(new Ui::Jobadd)
{
    ui->setupUi(this);
    ui->comboType->addItems({"H","F"});

    connect(ui->saveBtn, &QPushButton::clicked, this, [this]{
        const QString name = ui->editRoleName->text().trimmed();
        const QString type = ui->comboType->currentText();
        const double rate  = ui->spinRate->value();
        if (name.isEmpty() || rate<=0) { QMessageBox::warning(this,"Add Role","Enter a role and positive rate."); return; }

        const QString line = QString("%1$%2/%3/").arg(name).arg(rate, 0, 'f', 2).arg(type);
        auto roles = FileGateway::instance().readAll(Paths::file("jobs.txt")).value_or("");
        for (const auto &L : roles.split('\n')) if (L.trimmed()==line) { QMessageBox::information(this,"Add Role","Role already exists."); return; }
        FileGateway::instance().appendLine(Paths::file("jobs.txt"), line);
        accept();
    });

    connect(ui->closeBtn, &QPushButton::clicked, this, &QDialog::reject);
}
Jobadd::~Jobadd(){ delete ui; }
