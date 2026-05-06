#pragma once
#include <QDialog>
namespace Ui { class Jobadd; }
class Jobadd : public QDialog {
    Q_OBJECT
public:
    explicit Jobadd(QWidget* parent=nullptr);
    ~Jobadd();
private:
    Ui::Jobadd* ui;
};
