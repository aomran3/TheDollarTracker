#pragma once
#include <QDialog>

namespace Ui { class EmployeeWindow; }

class EmployeeWindow : public QDialog {
    Q_OBJECT
public:
    explicit EmployeeWindow(const QString& id, QWidget *parent = nullptr);
    ~EmployeeWindow();

private slots:
    void clockIn();
    void clockOut();
    void signOut();
    void viewMyShifts();

private:
    QString m_id;
    Ui::EmployeeWindow *ui;
};
