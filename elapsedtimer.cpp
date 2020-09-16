#include "elapsedtimer.h"

#include <QTimerEvent>

ElapsedTimer::ElapsedTimer(QObject *parent) : QObject(parent)
{}

QTime ElapsedTimer::elapsed() const
{
    return QTime::fromMSecsSinceStartOfDay(t_elapsed);
}

void ElapsedTimer::setRunning(bool running)
{
    if (m_running == running)
        return;

    m_running = running;
    emit runningChanged(running);

    if (m_running) {
        tid = startTimer(m_interval, Qt::PreciseTimer);
        t.restart();
    } else {
        killTimer(tid);
        tid = -1;
        t.invalidate();
    }
}

int ElapsedTimer::interval() const
{
    return m_interval;
}

void ElapsedTimer::setInterval(int interval)
{
    if (m_interval == interval)
        return;

    m_interval = interval;
    emit intervalChanged(interval);
}

void ElapsedTimer::start()
{
    setRunning(true);
}

void ElapsedTimer::pause()
{
    setRunning(false);
}

void ElapsedTimer::reset()
{
    t_elapsed = 0;
}

void ElapsedTimer::timerEvent(QTimerEvent *e)
{
    if (e->timerId() != tid || !t.isValid())
        return;
    t_elapsed += t.restart();
    emit elapsedChanged();
}

