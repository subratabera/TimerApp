#ifndef REMINDERDIALOG_H
#define REMINDERDIALOG_H

#include <QDialog>

class QPushButton;
class QLabel;

class ReminderDialog : public QDialog
{
    Q_OBJECT

public:
    // Explicit constructor with parent parameter
    explicit ReminderDialog(QWidget *parent = nullptr);
    ~ReminderDialog();

signals:
    void snoozeClicked();
    void dismissClicked();

private:
    QLabel *messageLabel;
    QPushButton *snoozeButton;
    QPushButton *dismissButton;
};

#endif // REMINDERDIALOG_H
