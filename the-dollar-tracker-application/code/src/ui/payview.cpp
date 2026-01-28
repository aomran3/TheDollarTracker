#include "payview.h"
#include "ui_payview.h"

#include "../core/Payroll.h"
#include "../core/FileGateway.h"
#include "../core/Paths.h"

#include <QDate>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

Payview::Payview(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::Payview)
{
    ui->setupUi(this);
    setWindowTitle("Weekly Payroll Summary");

    // Close button
    connect(ui->closeBtn, &QPushButton::clicked,
            this, &QDialog::accept);

    // Export CSV button
    connect(ui->exportCsvBtn, &QPushButton::clicked, this, [this] {
        if (m_lines.empty()) {
            QMessageBox::information(this, "Export Payroll",
                                     "No payroll data to export.");
            return;
        }

        QString fileName = QFileDialog::getSaveFileName(
            this,
            "Save Payroll CSV",
            "payroll_weekly.csv",
            "CSV Files (*.csv)"
            );

        if (fileName.isEmpty())
            return;

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, "Export Payroll",
                                  "Could not open file for writing.");
            return;
        }

        QTextStream out(&file);
        // Header row
        out << "EmployeeId,Hours,Rate,Pay\n";

        for (const Payroll::Line& l : m_lines) {
            out << l.empId << ","
                << QString::number(l.hours, 'f', 2) << ","
                << QString::number(l.rate,  'f', 2) << ","
                << QString::number(l.pay,   'f', 2) << "\n";
        }

        QMessageBox::information(this, "Export Payroll",
                                 "Payroll exported successfully.");
    });

    // ---- Load and display weekly payroll when dialog opens ----
    auto idsOpt  = FileGateway::instance().readAll(Paths::file("ids.txt"));
    auto logsOpt = FileGateway::instance().readAll(Paths::file("emp_clock.txt"));

    if (!idsOpt || !logsOpt) {
        QMessageBox::warning(this, "Payroll",
                             "Required payroll files not found.");
        return;
    }

    QDate today = QDate::currentDate();
    QDate weekStart = today.addDays(-today.dayOfWeek() + 1); // Monday

    // Compute payroll (weekly)
    m_lines = Payroll::computeWeekly(*idsOpt, *logsOpt, weekStart);

    QString report = Payroll::toText(m_lines);

    if (ui->outputEdit) {
        ui->outputEdit->setPlainText(report);
    }
}

Payview::~Payview()
{
    delete ui;
}
