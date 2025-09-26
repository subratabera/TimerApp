#ifndef TIMERAPP_H
#define TIMERAPP_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QTime>
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
    void showIntervalSetDialog();  // New slot for setting interval

private:
    void setupTrayIcon();
    void setupTimers();
    void startNextAlertTimer();  // Renamed from startNextHourTimer
    void startCountdown();
    void updateNextAlertLabel();
    void closeEvent(QCloseEvent *event) override;

    QSystemTrayIcon *trayIcon;
    QTimer *alertTimer;  // Renamed from hourlyTimer
    QTimer *snoozeTimer;
    QTimer *countdownTimer;
    ReminderDialog *reminderDialog;
    TimeSetDialog *timeSetDialog;
    IntervalSetDialog *intervalSetDialog;  // New dialog pointer
    QTime nextAlertTime;
    int customMinute;  // Store the custom minute (-1 means not set)
    int customInterval;  // New: Store the custom interval in minutes

    // UI elements
    QLabel *statusLabel;
    QLabel *nextAlertLabel;
    QLabel *countdownLabel;
    QPushButton *setTimeButton;
    QPushButton *setIntervalButton;  // New button for setting interval
    QPushButton *minimizeButton;
    QPushButton *quitButton;
};

#endif // TIMERAPP_H
