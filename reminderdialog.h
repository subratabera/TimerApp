#ifndef REMINDERDIALOG_H
#define REMINDERDIALOG_H

#include <QDialog>

class QPushButton;
class QLabel;

class ReminderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReminderDialog(QWidget *parent = nullptr);
    ~ReminderDialog();

    void setMessage(const QString &message);  // New method to update message

signals:
    void snoozeClicked();
    void dismissClicked();

private:
    QLabel *messageLabel;
    QPushButton *snoozeButton;
    QPushButton *dismissButton;
};

#endif // REMINDERDIALOG_H
