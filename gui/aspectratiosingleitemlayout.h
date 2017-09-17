/*
Copyright (c) 2017 Pavel Perina
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef ASPECTRATIOSINGLEITEMLAYOUT_H
#define ASPECTRATIOSINGLEITEMLAYOUT_H


class AspectRatioSingleItemLayout : public QLayout {
public:
    AspectRatioSingleItemLayout(double ratio = 16.0/9.0, QWidget *parent = nullptr)
        : QLayout(parent)
        , _item(nullptr)
        , _aspect_ratio(ratio)
    {
    }

    ~AspectRatioSingleItemLayout()
    {
        delete _item;
    }

    void setAspectRatio(double aspectRatio)
    {
        _aspect_ratio = aspectRatio;
    }

    int count() const
    {
        return _item != nullptr ? 1 : 0;
    }

    QLayoutItem *itemAt(int i) const
    {
        return i == 0 ? _item : nullptr;
    }

    QLayoutItem *takeAt(int)
    {
        QLayoutItem *retval = _item;
        _item = nullptr;
        return retval;
    }

    Qt::Orientations expandingDirections() const
    {
        // we'd like grow beyond sizeHint()
        return  Qt::Horizontal | Qt::Vertical;
    }

    bool hasHeightForWidth() const
    {
        return false;
    }

    int heightForWidth(int width) const
    {
        int height = (width - 2 * margin()) / _aspect_ratio + 2 * margin();
        return height;
    }

    void setGeometry(const QRect &rect)
    {
        QLayout::setGeometry(rect);
        if (_item) {
            QWidget *wdg = _item->widget();
            int availW = rect.width() - 2 * margin();
            int availH = rect.height() - 2 * margin();
            int w, h;
            h = availH;
            w = h * _aspect_ratio;
            if (w > availW) {
                // fill width
                w = availW;
                h = w / _aspect_ratio;
                int y;
                if (_item->alignment() & Qt::AlignTop) {
                    qDebug()<<"yes1\n";
                    y = margin();
            } else if (_item->alignment() & Qt::AlignBottom) {
                    qDebug()<<"yes2\n";
                    y = rect.height() - margin() - h;
                } else {
                    qDebug()<<"yes\n";
                    y = margin() + (availH - h) / 2;
                }
                wdg->setGeometry(rect.x() + margin(), rect.y() + y, w, h);
            }
            else {
                int x;
                if (_item->alignment() & Qt::AlignLeft)
                    x = margin();
                else if (_item->alignment() & Qt::AlignRight)
                    x = rect.width() - margin() - w;
                else
                    x = margin() + (availW - w) / 2;
                wdg->setGeometry(rect.x() + x, rect.y() + margin(), w, h);
            }
        }
    }

    QSize sizeHint() const
    {
        const int margins = 2 * margin();
        return _item ? _item->sizeHint() + QSize(margins, margins) : QSize(margins, margins);
    }

    QSize minimumSize() const
    {
        const int margins = 2 * margin();
        return _item ? _item->minimumSize() + QSize(margins, margins) : QSize(margins,margins);
    }

    void addItem(QLayoutItem *item)
    {
        delete _item;
        _item = item;
        item->setAlignment(0);
    }

private:

    QLayoutItem *_item; // at most one :-)

    double _aspect_ratio;

};
#endif // ASPECTRATIOSINGLEITEMLAYOUT_H
