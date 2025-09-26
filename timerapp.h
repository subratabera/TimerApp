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
    void showIntervalSetDialog();
    void editStatusText();  // New slot for editing status text

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
    QLabel *statusLabel;  // Changed back to QLabel
    QLabel *nextAlertLabel;
    QLabel *countdownLabel;
    QPushButton *setTimeButton;
    QPushButton *setIntervalButton;
    QPushButton *minimizeButton;
    QPushButton *quitButton;
    QPushButton *editStatusButton;  // New button for editing status
};

#endif // TIMERAPP_H
