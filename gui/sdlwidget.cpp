#include "sdlwidget.h"

SDLWidget::SDLWidget(QWidget *parent) :
    QWidget(parent), Screen()

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

SDLWidget::~SDLWidget() {
    delete[] _pixels;
}

void SDLWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    qDebug()<<"paint event\n";
    QImage image = QImage((uchar*)_pixels, WIDTH, HEIGHT, QImage::Format_ARGB32);

    painter.drawImage(0, 0, image);
}

void SDLWidget::updateContent(uword *memory) {
    qDebug()<<"drawing\n";
    for (unsigned int i = MEM_SCREEN_BEGIN; i <= 0xFFFF; ++i) {
        word pixel = (memory) ? memory[i] : 0;
        uint32_t blue = pixel & ((1<<5)-1);
        uint32_t green = (pixel>>5) & ((1<<5)-1);
        uint32_t red = (pixel>>10) ;
        _pixels[i - MEM_SCREEN_BEGIN] = ((red << (2+16)) + (green << (3+8)) + (blue << 3)) | 0xff000000;
    }
    this->update();
}

