
#ifndef ASPECTEDLAYOUT_H
#define ASPECTEDLAYOUT_H

#include <QLayout>
#include <QWidget>

/** @brief Layout to ensure central item has always same width to height ration. */
class AspectedLayout : public QLayout
{
    public:
    AspectedLayout();
    AspectedLayout(QWidget *parent);

    enum Position { CENTER, LEFT, RIGHT, TOP, BOTTOM };

    void addItem(QLayoutItem *item, Position pos);

    void recomputeSizes(const QRect &current);
    void setCentralWidget(QWidget *widget);
    void setRatio(int width, int height);
    virtual QSize sizeHint() const;

    virtual int heightForWidth(int w) const;
    virtual bool hasHeightForWidth() const;
    virtual void setGeometry(const QRect &rect);
    int indexOf(QWidget *widget);
    virtual QSize maximumSize() const;
    virtual QSize minimumSize() const;

    protected:
    virtual void addItem(QLayoutItem *item); 
    virtual int count() const;
    virtual QLayoutItem *itemAt(int index) const;
    virtual QLayoutItem *takeAt(int index);
    virtual Qt::Orientations expandingDirections() const;

    void createBorderWidgets();
    
    private:
    double  m_ratio; // width/height fraction to keep
    QLayoutItem *m_central;
    QLayoutItem *m_left;
    QLayoutItem *m_right;
    QLayoutItem *m_top;
    QLayoutItem *m_bottom;
};

#endif
