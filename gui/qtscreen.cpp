#include "qtscreen.h"

QtScreen::QtScreen(QWidget *parent) :
    QWidget(parent), Screen(), _last_time(0)

{
    _pixels = new QRgb[WIDTH*HEIGHT];
    _image = QImage((uchar*)_pixels, WIDTH, HEIGHT, QImage::Format_ARGB32);
    this->setMinimumHeight(HEIGHT);
    this->setMinimumWidth(WIDTH);
    this->resize(2*WIDTH, 2*HEIGHT);
    for (int i = 0; i < WIDTH * HEIGHT; ++i)
            _pixels[i] = 0xff00000;

        this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
}

QtScreen::~QtScreen() {
    delete[] _pixels;
}

void QtScreen::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    qDebug()<<"paint event\n";
    QImage image = QImage((uchar*)_pixels, WIDTH, HEIGHT, QImage::Format_ARGB32);

    QRectF rectangle;
    rectangle.setX(0);
    rectangle.setY(0);
    rectangle.setWidth(this->width());
    float ratio = (float)HEIGHT / (float)WIDTH;
    int height = ratio * this->width();
    rectangle.setHeight(height);
    painter.drawImage(rectangle, image);
}

void QtScreen::updateContent(uword *memory) {
    qDebug()<<"drawing\n";
    for (unsigned int i = MEM_SCREEN_BEGIN; i <= 0xFFFF; ++i) {
        word pixel = (memory) ? memory[i] : 0;
        uint32_t blue = pixel & ((1<<5)-1);
        uint32_t green = (pixel>>5) & ((1<<5)-1);
        uint32_t red = (pixel>>10) ;
        _pixels[i - MEM_SCREEN_BEGIN] = ((red << (2+16)) + (green << (3+8)) + (blue << 3)) | 0xff000000;
    }
    this->update();
    int curr_time = _timer.msec();
    if (curr_time - _last_time < (1000.f * 1.f/60.f))
        QThread::msleep((1000.f * 1.f / 60.f) - _last_time + curr_time);
   _last_time = curr_time;
}

