#include "ManagerWindow.h"
#include "ui_ManagerWindow.h"

#include "../core/Paths.h"
#include "../core/FileGateway.h"
#include "../core/Payroll.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QDate>
#include <QTime>

namespace {
// Small helpers to keep code tidy
QString askText(QWidget* parent, const QString& title, const QString& label, const QString& defVal = QString()) {
    bool ok = false;
    const QString v = QInputDialog::getText(parent, title, label, QLineEdit::Normal, defVal, &ok).trimmed();
    return ok ? v : QString();
}
bool ensureNonEmpty(QWidget* parent, const QString& s, const QString& what){
    if (s.isEmpty()) {
        QMessageBox::warning(parent, "Missing", what + " is required.");
        return false;
    }
    return true;
}
QStringList nonEmptyLines(const QString& txt){
    QStringList out;
    const QStringList lines = txt.split('\n');
    for (const QString& L : lines) {
        const QString t = L.trimmed();
        if (!t.isEmpty()) out << t;
    }
    return out;
}
}

ManagerWindow::ManagerWindow(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::ManagerWindow)
{
    ui->setupUi(this);

    connect(ui->btnViewEmployees,    &QPushButton::clicked, this, &ManagerWindow::viewEmployees);
    connect(ui->btnViewSchedule,     &QPushButton::clicked, this, &ManagerWindow::viewSchedule);
    connect(ui->btnPreviewPayroll,   &QPushButton::clicked, this, &ManagerWindow::previewPayroll);
    connect(ui->btnExportPayroll,    &QPushButton::clicked, this, &ManagerWindow::exportPayroll);
    connect(ui->btnSignout,          &QPushButton::clicked, this, &ManagerWindow::signOut);
    connect(ui->btnAddEmp,           &QPushButton::clicked, this, &ManagerWindow::addEmployee);
    connect(ui->btnRemoveEmp,        &QPushButton::clicked, this, &ManagerWindow::removeEmployee);
    connect(ui->btnAddMgr,           &QPushButton::clicked, this, &ManagerWindow::addManager);
    connect(ui->btnRemoveMgr,        &QPushButton::clicked, this, &ManagerWindow::removeManager);
    connect(ui->btnAddRole,          &QPushButton::clicked, this, &ManagerWindow::addRole);
    connect(ui->btnRemoveRole,       &QPushButton::clicked, this, &ManagerWindow::removeRole);
    connect(ui->btnListRoles,        &QPushButton::clicked, this, &ManagerWindow::listRoles);
    connect(ui->btnAssignRole,       &QPushButton::clicked, this, &ManagerWindow::assignRole);
    connect(ui->btnCreateShift,      &QPushButton::clicked, this, &ManagerWindow::createShift);
    connect(ui->btnViewLogs,         &QPushButton::clicked, this, &ManagerWindow::viewLogs);
    connect(ui->btnFilterSchedule,   &QPushButton::clicked, this, &ManagerWindow::filterSchedule);
}

ManagerWindow::~ManagerWindow()
{
    delete ui;
}

// =========================
//  Employee List Viewer
// =========================
void ManagerWindow::viewEmployees()
{
    auto text = FileGateway::instance().readAll(Paths::file("ids.txt"));
    ui->plain->setPlainText(text.value_or("⚠️ Could not load ids.txt"));
}

// =========================
//  Schedule Viewer
// =========================
void ManagerWindow::viewSchedule()
{
    auto text = FileGateway::instance().readAll(Paths::file("schedule.txt"));
    ui->plain->setPlainText(text.value_or("⚠️ Could not load schedule.txt"));
}

// =========================
//  Preview Payroll
// =========================
void ManagerWindow::previewPayroll()
{
    const QString ids  = FileGateway::instance().readAll(Paths::file("ids.txt")).value_or("");
    const QString logs = FileGateway::instance().readAll(Paths::file("emp_clock.txt")).value_or("");

    const auto computed = Payroll::compute(ids, logs);
    ui->plain->setPlainText(Payroll::toText(computed));
}

// =========================
//  Export Payroll to file
// =========================
void ManagerWindow::exportPayroll()
{
    const QString ids  = FileGateway::instance().readAll(Paths::file("ids.txt")).value_or("");
    const QString logs = FileGateway::instance().readAll(Paths::file("emp_clock.txt")).value_or("");

    const auto computed = Payroll::compute(ids, logs);
    const bool ok = FileGateway::instance().writeAll(
        Paths::file("PayStubs.txt"),
        Payroll::toText(computed)
        );

    QMessageBox::information(
        this,
        "Payroll Export",
        ok ? "✅ PayStubs.txt has been generated." : "❌ Unable to write PayStubs.txt"
        );
}

// =========================
//  Add / Remove Employees
// =========================
void ManagerWindow::addEmployee()
{
    const QString id = askText(this, "Add Employee", "Employee ID (e.g., emp312):");
    if (!ensureNonEmpty(this, id, "Employee ID")) return;

    const QString roleLine = askText(this, "Assign Role (optional)",
                                     "Enter role line from jobs.txt (e.g., Service$20/H/), or leave blank:");
    QString newLine = id;
    if (!roleLine.trimmed().isEmpty()) newLine += ", " + roleLine.trimmed();
    else                               newLine += ", Unassigned$0/H/";

    auto t = FileGateway::instance().readAll(Paths::file("ids.txt")).value_or("");
    if (!t.endsWith('\n') && !t.isEmpty()) t += '\n';
    t += newLine + '\n';
    FileGateway::instance().writeAll(Paths::file("ids.txt"), t);

    QMessageBox::information(this, "Add Employee", "Added: " + newLine);
    viewEmployees();
}

void ManagerWindow::removeEmployee()
{
    const QString id = askText(this, "Remove Employee", "Employee ID to remove:");
    if (!ensureNonEmpty(this, id, "Employee ID")) return;

    auto t  = FileGateway::instance().readAll(Paths::file("ids.txt")).value_or("");
    auto ls = nonEmptyLines(t);
    QStringList out;
    bool removed = false;
    for (const QString& L : ls) {
        if (L.trimmed().startsWith(id + ",")) { removed = true; continue; }
        out << L;
    }
    FileGateway::instance().writeAll(Paths::file("ids.txt"), out.join('\n') + '\n');
    QMessageBox::information(this, "Remove Employee", removed ? "Removed " + id : "ID not found.");
    viewEmployees();
}

// =========================
//  Add / Remove Managers
// =========================
void ManagerWindow::addManager()
{
    const QString id = askText(this, "Add Manager", "Manager ID (e.g., mg3307):");
    if (!ensureNonEmpty(this, id, "Manager ID")) return;

    auto t = FileGateway::instance().readAll(Paths::file("mg_ids.txt")).value_or("");
    if (!t.endsWith('\n') && !t.isEmpty()) t += '\n';
    t += id + '\n';
    FileGateway::instance().writeAll(Paths::file("mg_ids.txt"), t);
    QMessageBox::information(this, "Add Manager", "Added: " + id);
}

void ManagerWindow::removeManager()
{
    const QString id = askText(this, "Remove Manager", "Manager ID to remove:");
    if (!ensureNonEmpty(this, id, "Manager ID")) return;

    auto t  = FileGateway::instance().readAll(Paths::file("mg_ids.txt")).value_or("");
    auto ls = nonEmptyLines(t);
    QStringList out;
    bool removed = false;
    for (const QString& L : ls) {
        if (L.trimmed() == id) { removed = true; continue; }
        out << L;
    }
    FileGateway::instance().writeAll(Paths::file("mg_ids.txt"), out.join('\n') + '\n');
    QMessageBox::information(this, "Remove Manager", removed ? "Removed " + id : "ID not found.");
}

// =========================
//  Roles CRUD + List
// Format: RoleName$Rate/Type/
// =========================
void ManagerWindow::addRole()
{
    const QString name = askText(this, "Add Role", "Role name (e.g., Service):");
    if (!ensureNonEmpty(this, name, "Role name")) return;

    const QString rate = askText(this, "Add Role", "Rate (number):");
    if (!ensureNonEmpty(this, rate, "Rate")) return;

    const QString type = askText(this, "Add Role", "Type (H or F):", "H");
    if (type != "H" && type != "F") {
        QMessageBox::warning(this, "Add Role", "Type must be H (hourly) or F (flat).");
        return;
    }

    const QString line = QString("%1$%2/%3/").arg(name.trimmed(), rate.trimmed(), type);
    auto t = FileGateway::instance().readAll(Paths::file("jobs.txt")).value_or("");
    if (!t.endsWith('\n') && !t.isEmpty()) t += '\n';
    t += line + '\n';
    FileGateway::instance().writeAll(Paths::file("jobs.txt"), t);
    QMessageBox::information(this, "Add Role", "Added: " + line);
}

void ManagerWindow::removeRole()
{
    const QString roleLine = askText(this, "Remove Role",
                                     "Paste exact role line (e.g., Service$20/H/):");
    if (!ensureNonEmpty(this, roleLine, "Role")) return;

    auto t  = FileGateway::instance().readAll(Paths::file("jobs.txt")).value_or("");
    auto ls = nonEmptyLines(t);
    QStringList out;
    bool removed = false;
    for (const QString& L : ls) {
        if (L.trimmed() == roleLine.trimmed()) { removed = true; continue; }
        out << L;
    }
    FileGateway::instance().writeAll(Paths::file("jobs.txt"), out.join('\n') + '\n');
    QMessageBox::information(this, "Remove Role", removed ? "Removed." : "Role not found.");
}

void ManagerWindow::listRoles()
{
    const QString t = FileGateway::instance().readAll(Paths::file("jobs.txt")).value_or("No roles.");
    ui->plain->setPlainText(t);
}

// =========================
//  Assign role to employee
// =========================
void ManagerWindow::assignRole()
{
    const QString id = askText(this, "Assign Role", "Employee ID (e.g., emp312):");
    if (!ensureNonEmpty(this, id, "Employee ID")) return;

    const QString role = askText(this, "Assign Role",
                                 "Role line from jobs.txt (e.g., Service$20/H/):");
    if (!ensureNonEmpty(this, role, "Role line")) return;

    auto t  = FileGateway::instance().readAll(Paths::file("ids.txt")).value_or("");
    auto ls = nonEmptyLines(t);
    bool updated = false;
    for (QString& L : ls) {
        if (L.startsWith(id + ",")) {
            L = id + ", " + role.trimmed();
            updated = true;
        }
    }
    if (!updated) {
        QMessageBox::warning(this, "Assign Role", "Employee ID not found in ids.txt");
        return;
    }
    FileGateway::instance().writeAll(Paths::file("ids.txt"), ls.join('\n') + '\n');
    QMessageBox::information(this, "Assign Role", "Assigned role to " + id);
    viewEmployees();
}

// =========================
//  Create shift
// Format in schedule.txt:
//
// -----------------------------------
// dd/MM/yyyy
// emp312     10:00 AM - 5:00PM
// -----------------------------------
//
// =========================
void ManagerWindow::createShift()
{
    const QString id   = askText(this, "Create Shift", "Employee ID (e.g., emp312):");
    if (!ensureNonEmpty(this, id, "Employee ID")) return;

    const QString dateStr  = askText(this, "Create Shift", "Date (dd/MM/yyyy):", QDate::currentDate().toString("dd/MM/yyyy"));
    if (!ensureNonEmpty(this, dateStr, "Date")) return;

    const QString startStr = askText(this, "Create Shift", "Start time (e.g., 9:00 AM):", QTime::currentTime().toString("h:mm AP"));
    if (!ensureNonEmpty(this, startStr, "Start time")) return;

    const QString endStr   = askText(this, "Create Shift", "End time (e.g., 5:00 PM):", QTime::currentTime().toString("h:mm AP"));
    if (!ensureNonEmpty(this, endStr, "End time")) return;

    QString t = FileGateway::instance().readAll(Paths::file("schedule.txt")).value_or("");
    if (!t.endsWith('\n') && !t.isEmpty()) t += '\n';

    QStringList lines = t.split('\n');
    bool inserted = false;
    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i].trimmed() == dateStr.trimmed()) {
            // Insert after date header
            lines.insert(i + 1, QString("%1     %2 - %3").arg(id, startStr, endStr));
            inserted = true;
            break;
        }
    }
    if (!inserted) {
        lines << "-----------------------------------";
        lines << dateStr;
        lines << QString("%1     %2 - %3").arg(id, startStr, endStr);
    }
    const bool ok = FileGateway::instance().writeAll(Paths::file("schedule.txt"), lines.join('\n') + '\n');
    QMessageBox::information(this, "Create Shift", ok ? "Shift saved." : "Failed to write schedule.txt");
    viewSchedule();
}

// =========================
//  View time logs (emp_clock.txt)
// =========================
void ManagerWindow::viewLogs()
{
    const QString t = FileGateway::instance().readAll(Paths::file("emp_clock.txt")).value_or("No logs.");
    ui->plain->setPlainText(t);
}

// =========================
//  Sign Out returning to main menu
// =========================
void ManagerWindow::signOut() {
    accept();
}

// =========================
//  Filter schedule by employee or date range
// =========================
void ManagerWindow::filterSchedule()
{
    const QString byEmp = askText(this, "Filter Schedule", "Filter by Employee ID (blank = no filter):");
    const QString fromS = askText(this, "Filter Schedule", "From date (dd/MM/yyyy) blank=none:");
    const QString toS   = askText(this, "Filter Schedule", "To date (dd/MM/yyyy) blank=none:");

    const QDate from = fromS.isEmpty() ? QDate() : QDate::fromString(fromS, "dd/MM/yyyy");
    const QDate to   = toS.isEmpty()   ? QDate() : QDate::fromString(toS,   "dd/MM/yyyy");

    const QString all = FileGateway::instance().readAll(Paths::file("schedule.txt")).value_or("");
    const QStringList lines = all.split('\n');

    QString out;
    QString currentDateStr;
    for (const QString& L : lines) {
        const QString t = L.trimmed();
        if (t == "-----------------------------------") {
            out += L + '\n';
            continue;
        }
        // A date line looks like dd/MM/yyyy
        const QDate maybe = QDate::fromString(t, "dd/MM/yyyy");
        if (maybe.isValid()) {
            currentDateStr = t;
            // date-range filter
            if (!from.isNull() && !to.isNull()) {
                if (maybe < from || maybe > to) {
                    // mark we are out-of-range by setting a sentinel; skip until next separator
                    currentDateStr = "SKIP";
                }
            } else if (!from.isNull() && maybe < from) {
                currentDateStr = "SKIP";
            } else if (!to.isNull() && maybe > to) {
                currentDateStr = "SKIP";
            }
            if (currentDateStr != "SKIP") out += L + '\n';
            continue;
        }

        // shift lines (“emp312     10:00 AM - 5:00PM”)
        if (currentDateStr == "SKIP") continue;
        if (!byEmp.isEmpty() && !t.startsWith(byEmp + " ")) continue;
        out += L + '\n';
    }

    ui->plain->setPlainText(out.isEmpty() ? "No matches." : out);
}
