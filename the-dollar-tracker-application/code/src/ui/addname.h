#pragma once
#include <QDialog>
namespace Ui { class Addname; }
class Addname : public QDialog {
    Q_OBJECT
public:
    explicit Addname(QWidget* parent=nullptr);
    ~Addname();
private:
    Ui::Addname* ui;
};
