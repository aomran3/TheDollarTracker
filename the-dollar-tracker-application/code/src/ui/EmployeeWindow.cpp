#include "EmployeeWindow.h"
#include "ui_EmployeeWindow.h"

#include "../core/Paths.h"
#include "../core/FileGateway.h"
#include "../core/Validation.h"

#include <QMessageBox>
#include <QDateTime>
#include <QDialog>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QLabel>

// Helper to update status label if present
static void setStatus(QWidget* w, const QString& text)
{
    if (auto* lbl = w->findChild<QLabel*>("lblStatus")) {
        lbl->setText(text);
    }
}

EmployeeWindow::EmployeeWindow(const QString& id, QWidget* parent)
    : QDialog(parent),
    m_id(id),
    ui(new Ui::EmployeeWindow)
{
    ui->setupUi(this);
    setWindowTitle(QString("Employee — %1").arg(m_id));

    connect(ui->btnIn,         &QPushButton::clicked, this, &EmployeeWindow::clockIn);
    connect(ui->btnOut,        &QPushButton::clicked, this, &EmployeeWindow::clockOut);
    connect(ui->btnSignOut,    &QPushButton::clicked, this, &EmployeeWindow::signOut);
    connect(ui->btnViewShifts, &QPushButton::clicked, this, &EmployeeWindow::viewMyShifts);

    setStatus(this, "Status: Ready");
}

EmployeeWindow::~EmployeeWindow()
{
    delete ui;
}

// Prevent double clock-in
void EmployeeWindow::clockIn()
{
    auto txtOpt = FileGateway::instance().readAll(Paths::file("emp_clock.txt"));
    if (txtOpt) {
        QStringList lines = txtOpt->split('\n');
        for (int i = lines.size() - 1; i >= 0; --i) {
            QString L = lines[i].trimmed();
            if (!L.startsWith(m_id)) continue;

            if (L.endsWith("/*")) {
                QMessageBox::warning(this, "Clock In",
                                     "You are already clocked in.");
                setStatus(this, "Status: Already clocked in");
                return;
            }
            break;
        }
    }

    const auto now = QDateTime::currentDateTime();
    const QString line =
        QString("%1, %2 -%3/*")
            .arg(m_id,
                 now.date().toString("yyyy.MM.dd"),
                 now.time().toString("HH:mm:ss"));

    FileGateway::instance().appendLine(Paths::file("emp_clock.txt"), line);

    setStatus(this,
              QString("Status: Clocked in at %1")
                  .arg(now.time().toString("hh:mm:ss AP")));

    QMessageBox::information(this, "Clock In", "Clock-in successful.");
}

void EmployeeWindow::clockOut()
{
    auto txtOpt = FileGateway::instance().readAll(Paths::file("emp_clock.txt"));
    if (!txtOpt) {
        QMessageBox::warning(this, "Clock Out", "Clock log not found.");
        setStatus(this, "Status: Clock log not found");
        return;
    }

    QStringList lines = txtOpt->split('\n');
    for (int i = lines.size() - 1; i >= 0; --i) {
        QString L = lines[i].trimmed();
        if (!L.startsWith(m_id)) continue;
        if (!L.endsWith("/*")) continue;

        const auto now = QDateTime::currentDateTime();
        L.chop(2);
        L += "/" + now.time().toString("HH:mm:ss");
        lines[i] = L;

        FileGateway::instance()
            .writeAll(Paths::file("emp_clock.txt"), lines.join('\n'));

        setStatus(this,
                  QString("Status: Clocked out at %1")
                      .arg(now.time().toString("hh:mm:ss AP")));

        QMessageBox::information(this, "Clock Out", "Clock-out successful.");
        return;
    }

    QMessageBox::information(this, "Clock Out",
                             "No open session to close.");
    setStatus(this, "Status: No open session");
}

void EmployeeWindow::signOut()
{
    accept();
}

void EmployeeWindow::viewMyShifts()
{
    const QString sched =
        FileGateway::instance()
            .readAll(Paths::file("schedule.txt"))
            .value_or("");

    if (sched.trimmed().isEmpty()) {
        QMessageBox::information(this, "My Shifts", "No schedule found.");
        return;
    }

    QString out;
    QString currentDate;
    const QStringList lines = sched.split('\n');

    for (int i = 0; i < lines.size(); ++i) {
        const QString L = lines[i].trimmed();

        if (L.startsWith('-') && i + 1 < lines.size()) {
            currentDate = lines[i + 1].trimmed();
            continue;
        }

        if (L.contains(m_id)) {
            if (!currentDate.isEmpty())
                out += currentDate + "\n";
            out += L + "\n\n";
        }
    }

    if (out.trimmed().isEmpty())
        out = "No shifts assigned.";

    QDialog dlg(this);
    dlg.setWindowTitle("My Shifts");
    QVBoxLayout layout(&dlg);
    QPlainTextEdit view;
    view.setReadOnly(true);
    view.setPlainText(out);
    layout.addWidget(&view);
    dlg.resize(520, 380);
    dlg.exec();
}
