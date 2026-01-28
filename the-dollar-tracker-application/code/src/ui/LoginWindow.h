#pragma once
#include <QDialog>
namespace Ui { class LoginWindow; }
class LoginWindow : public QDialog {
    Q_OBJECT
public:
    explicit LoginWindow(QWidget* parent=nullptr);
    ~LoginWindow();
    QString userId() const;
private:
    Ui::LoginWindow* ui;
};
