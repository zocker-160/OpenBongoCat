
#include <QApplication>
#include <QMainWindow>
#include <QDialog>

#include <QLabel>
#include <QPushButton>
#include <QImage>
#include <QPixmap>

#include <QPainter>
#include <QPaintEvent>

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QKeyEvent>
#include <QTimer>

#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QSoundEffect>

#include <iostream>


#include "keylogger.h"


#define APPNAME "OpenBongoCat"

#define ASSETS "/home/bene/tmp/Xtest/assets"

#define SCREAM "sound/dscream3.wav"

#define CAT_TMP "img/cat_old.png"

#define CAT "img/cat.png"
#define PAW_LEFT "img/paw-left.png"
#define PAW_RIGHT "img/paw-right.png"
#define BONGO "img/bongo.png"


#define PAW_ANIMATION 50 // in ms


#define WIDTH 450
#define HEIGHT 400

#define IMG_WIDTH 800
#define IMG_HEIGHT 450


QString getAsset(QString filename) {
    return QDir(ASSETS).absoluteFilePath(filename);
}

enum class PAW {
    Left,
    Right
};


class MainWindow : public QMainWindow {

public:
    explicit MainWindow() : QMainWindow() {
        setWindowTitle(APPNAME);
        
        setAttribute(Qt::WA_AlwaysStackOnTop);
        setAttribute(Qt::WA_TranslucentBackground);
        setStyleSheet("background: transparent;");

#if 1
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
#else
        setWindowFlags(Qt::WindowStaysOnTopHint);
#endif

        bongoP.setSource(QUrl::fromLocalFile(getAsset("sound/bongo1.wav")));
        bongoS.setSource(QUrl::fromLocalFile(getAsset("sound/bongo0.wav")));
    }

    ~MainWindow() {
        delete cat_old;
        delete cat_bg;
        delete cat;
        delete lPaw_up;
        delete lPaw_down;
        delete rPaw_up;
        delete rPaw_up_bg;
        delete rPaw_down;
        delete mouth;
        delete mouth_open;
    }

public slots:
    void playSound() {
        qDebug() << "button pressed";

        auto sound = new QSoundEffect(this);
        sound->setSource(QDir(ASSETS).absoluteFilePath(SCREAM));
        sound->play();

        connect(sound, &QSoundEffect::playingChanged, sound, &QSoundEffect::deleteLater);
        /*
        connect(sound, &QSoundEffect::playingChanged, [sound] {
            qDebug() << "delete later";
            sound->deleteLater();
        });
        */
    }

    void keyInput() {
        qDebug() << "key input";

        clickCounter++;

        if (pawSequence == PAW::Left && !rightDown) {
            rightDown = true;
            pawSequence = PAW::Right;

            update();
            bongoP.play();

            QTimer::singleShot(PAW_ANIMATION, [=] {
                rightDown = false;
                update();
            });
        }
        else if (pawSequence == PAW::Right && !leftDown) {
            leftDown = true;
            pawSequence = PAW::Left;

            update();
            bongoS.play();

            QTimer::singleShot(PAW_ANIMATION, [=] {
                leftDown = false;
                update();
            });
        }
    }


protected:
    void keyPressEvent(QKeyEvent* event) override {
        if (event->key() == Qt::Key_Escape) {
            qDebug() << "ESC";
            QApplication::exit();
        }

        return;

        if (event->key() == Qt::Key_A) {
            rightDown = true;
        }
        if (event->key() == Qt::Key_D) {
            leftDown = true;
        }

        update();
    }

    void keyReleaseEvent(QKeyEvent* event) override {
        return;

        if (event->key() == Qt::Key_A) {
            rightDown = false;
        }
        if (event->key() == Qt::Key_D) {
            leftDown = false;
        }

        update();
    }

    void paintEvent(QPaintEvent* event) override {
        //qDebug() << "paint event";

        QPainter painter(this);

        if (rightDown) {
            drawPart(painter, *cat_bg);
            drawPart(painter, *cat);

            drawInstrument(painter);
            drawPart(painter, *rPaw_down);
        }
        else {
            drawInstrument(painter);

            drawPart(painter, *rPaw_up_bg);
            drawPart(painter, *cat_bg);
            drawPart(painter, *cat);
            drawPart(painter, *rPaw_up);
        }

        if (leftDown) {
            drawPart(painter, *lPaw_down);
        }
        else {
            drawPart(painter, *lPaw_up);
        }

        // mouth open
        drawPart(painter, *mouth);

        drawClickCounter(painter);
    }

private:
    PAW pawSequence = PAW::Left;
    bool leftDown = false;
    bool rightDown = false;

    int clickCounter = 0;

    QSoundEffect bongoP;
    QSoundEffect bongoS;

    QImage* cat_old = new QImage(getAsset("img/data/009.png"));

    QImage* cat_bg = new QImage(getAsset("img/data/007.png"));
    QImage* cat = new QImage(getAsset("img/data/006.png"));

    QImage* lPaw_up = new QImage(getAsset("img/data/002.png"));
    QImage* lPaw_down = new QImage(getAsset("img/data/003.png"));

    QImage* rPaw_up_bg = new QImage(getAsset("img/data/008.png"));
    QImage* rPaw_up = new QImage(getAsset("img/data/004.png"));

    QImage* rPaw_down = new QImage(getAsset("img/data/005.png"));

    QImage* mouth = new QImage(getAsset("img/data/000.png"));
    QImage* mouth_open = new QImage(getAsset("img/data/001.png"));

    QImage* bongo = new QImage(getAsset(BONGO));


    void drawPart(QPainter& painter, QImage& image) {
        painter.drawImage(0, 0, image, 260, 0, WIDTH, HEIGHT);
    }

    void drawInstrument(QPainter& painter) {
        drawPart(painter, *bongo);
    }

    void drawClickCounter(QPainter& painter) {
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);

        QRect rect(100, 270, 200, 30);
        QPen pen(Qt::black);
        pen.setWidth(4);

        QFont font;
        font.setBold(true);
        font.setPixelSize(20);

        painter.setFont(font);
        painter.setPen(pen);
        painter.setBrush(Qt::gray);

        painter.drawRect(rect);
        painter.drawText(rect, Qt::AlignCenter, QString::number(clickCounter));
    }
};


int main(int argc, char* argv[]) {
    std::cout << "OpenBongoCat by zocker_160" << std::endl;

    QApplication app(argc, argv);
#if 0
    GlobalKeylogger keylogger(nullptr, false);
    if (keylogger.hasFailed()) {
        qDebug() << "failed to init Xkeylogger";
        return 1;
    }
    else {
        keylogger.start();
    }

    QObject::connect(
        &keylogger, &GlobalKeylogger::keyPressed,
        &keylogger, &GlobalKeylogger::test
    );
#endif

    MainWindow dial;
    dial.setFixedSize(WIDTH, HEIGHT);
    dial.show();

    //GlobalKeylogger logger(&dial, false);
    auto logger = new GlobalKeylogger(&dial, false);
    if (logger->hasFailed()) {
        qDebug() << "failed to load X logger";
        return -1;
    }
    else {
        QObject::connect(
            logger, &GlobalKeylogger::keyPressed,
            &dial, &MainWindow::keyInput
        );
        QObject::connect(
            logger, &GlobalKeylogger::mousePressed,
            &dial, &MainWindow::keyInput
        );

        //qDebug() << "starting";
        logger->start();
    }

    QObject::connect(
        &app, &QApplication::aboutToQuit, [logger] {
            logger->stop();
            logger->wait();
        }
    );

    return QApplication::exec();
}
