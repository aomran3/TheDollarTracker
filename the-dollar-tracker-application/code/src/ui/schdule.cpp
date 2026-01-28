#include "schdule.h"
#include "ui_schdule.h"

#include "../core/FileGateway.h"
#include "../core/Paths.h"
#include "../core/Validation.h"

#include <QMessageBox>
#include <QDateTime>

Schdule::Schdule(QWidget* parent)
    : QDialog(parent),
    ui(new Ui::Schdule)
{
    ui->setupUi(this);
    setWindowTitle("Create Shift");

    connect(ui->saveBtn, &QPushButton::clicked, this, [this] {
        const QString id = ui->editEmpId->text().trimmed();
        const QDate date = ui->dateEdit->date();
        const QTime startT = ui->timeStart->time();
        const QTime endT   = ui->timeEnd->time();

        if (!Validation::isValidEmployeeId(id)) {
            QMessageBox::warning(this, "Create Shift",
                                 "Employee ID must be 4–8 digits.");
            return;
        }

        QDateTime start(date, startT);
        QDateTime end(date, endT);

        if (!Validation::isValidShiftTimes(start, end)) {
            QMessageBox::warning(this, "Create Shift",
                                 "End time must be after start time and "
                                 "shift length must be reasonable.");
            return;
        }

        const QString dateStr  = date.toString("dd/MM/yyyy");
        const QString startStr = startT.toString("h:mm AP");
        const QString endStr   = endT.toString("h:mm AP");

        QString sched =
            FileGateway::instance()
                .readAll(Paths::file("schedule.txt"))
                .value_or("");

        QStringList lines =
            sched.split('\n', Qt::KeepEmptyParts);

        int insertAt = -1;
        for (int i = 0; i < lines.size(); ++i) {
            if (lines[i].trimmed() == dateStr)
                insertAt = i + 1;
        }

        if (insertAt < 0) {
            lines << "-----------------------------------";
            lines << dateStr;
            insertAt = lines.size();
        }

        lines.insert(insertAt,
                     QString("%1     %2 - %3")
                         .arg(id, -10)
                         .arg(startStr)
                         .arg(endStr));

        FileGateway::instance()
            .writeAll(Paths::file("schedule.txt"),
                      lines.join('\n'));

        accept();
    });

    connect(ui->closeBtn, &QPushButton::clicked,
            this, &QDialog::reject);
}

Schdule::~Schdule()
{
    delete ui;
}
