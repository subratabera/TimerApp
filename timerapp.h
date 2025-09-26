#ifndef TIMERAPP_H
#define TIMERAPP_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QTime>
#include <QLabel>

class ReminderDialog;

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

private:
    void setupTrayIcon();
    void setupTimers();
    void startNextHourTimer();
    void startCountdown();
    void updateNextAlertLabel();
    void closeEvent(QCloseEvent *event) override;

    QSystemTrayIcon *trayIcon;
    QTimer *hourlyTimer;
    QTimer *snoozeTimer;
    QTimer *countdownTimer;
    ReminderDialog *reminderDialog;
    QTime nextAlertTime;

    // UI elements
    QLabel *statusLabel;
    QLabel *nextAlertLabel;
    QLabel *countdownLabel;
};

#endif // TIMERAPP_H
