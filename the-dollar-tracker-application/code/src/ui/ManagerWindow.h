#pragma once
#include <QDialog>
namespace Ui { class ManagerWindow; }
class ManagerWindow : public QDialog {
    Q_OBJECT
public:
    explicit ManagerWindow(QWidget* parent=nullptr);
    ~ManagerWindow();
private slots:
    void viewEmployees();
    void viewSchedule();
    void previewPayroll();
    void exportPayroll();
    void addEmployee();
    void removeEmployee();
    void addManager();
    void removeManager();
    void addRole();
    void removeRole();
    void listRoles();
    void assignRole();
    void createShift();
    void viewLogs();
    void filterSchedule();
    void signOut();
private:
    Ui::ManagerWindow* ui;
};
