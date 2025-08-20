#pragma once

#include <QObject>
#include <QThread>

#include <X11/Xlib.h>


class GlobalKeylogger : public QThread {
    Q_OBJECT

public:
    explicit GlobalKeylogger(QObject* parent, bool logMouseClicks);
    ~GlobalKeylogger();

    bool hasFailed();
    void stop();

/*
public slots:
    void test();
*/

signals:
    void keyPressed();
    void mousePressed();
    //void mousePressed(int button);

private:
    Display* display;
    int xi_opcode;

    bool initFailed = false;
    bool shouldStop = false;


protected:
    void run() override;

};
