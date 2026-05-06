#pragma once
#include <QDialog>
namespace Ui { class Removename; }
class Removename : public QDialog {
    Q_OBJECT
public:
    explicit Removename(QWidget* parent=nullptr);
    ~Removename();
private:
    Ui::Removename* ui;
};
