#ifndef TIMERAPP_H
#define TIMERAPP_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QTime>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

class ReminderDialog;
class TimeSetDialog;
class IntervalSetDialog;

class TimerApp : public QMainWindow
{
    Q_OBJECT

public:
    TimerApp(QWidget *parent = nullptr);
    ~TimerApp();

private slots:
    void showReminder();
    void snoozeReminder();
    void dismissReminder();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void quitApp();
    void showMainWindow();
    void updateCountdown();
    void showTimeSetDialog();
    void showIntervalSetDialog();
    void onStatusTextChanged();  // New slot for status text changes

private:
    void setupTrayIcon();
    void setupTimers();
    void startNextAlertTimer();
    void startCountdown();
    void updateNextAlertLabel();
    void closeEvent(QCloseEvent *event) override;

    QSystemTrayIcon *trayIcon;
    QTimer *alertTimer;
    QTimer *snoozeTimer;
    QTimer *countdownTimer;
    ReminderDialog *reminderDialog;
    TimeSetDialog *timeSetDialog;
    IntervalSetDialog *intervalSetDialog;
    QTime nextAlertTime;
    int customMinute;
    int customInterval;

    // UI elements
    QLineEdit *statusEdit;  // Changed from QLabel to QLineEdit
    QLabel *nextAlertLabel;
    QLabel *countdownLabel;
    QPushButton *setTimeButton;
    QPushButton *setIntervalButton;
    QPushButton *minimizeButton;
    QPushButton *quitButton;
};

#endif // TIMERAPP_H
