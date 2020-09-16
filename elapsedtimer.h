#ifndef ELAPSEDTIMER_H
#define ELAPSEDTIMER_H
#include <QTimer>
#include <QObject>
#include <QElapsedTimer>
#include <QTime>
class ElapsedTimer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QTime elapsed READ elapsed NOTIFY elapsedChanged)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)

    qint64 t_elapsed = 0;
    int m_interval = 100;
    int tid = -1;
    bool m_running = false;
    QElapsedTimer t;


public:
    explicit ElapsedTimer(QObject *parent = 0);

    QTime elapsed() const;

    inline bool running() const { return m_running; }

    void setRunning(bool running);

    int interval() const;
    void setInterval(int interval);

signals:
    void elapsedChanged();
    void runningChanged(bool running);
    void intervalChanged(int interval);

public slots:
    void start();
    void pause();
    void reset();

protected:
    void timerEvent(QTimerEvent *e);
};

#endif // ELAPSEDTIMER_H
