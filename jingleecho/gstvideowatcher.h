
#ifndef GSTVIDEOWATCHER_H
#define GSTVIDEOWATCHER_H

class GstVideoWatcher
{
    public:

    virtual void handleExpose() = 0;
    void videoResolutionChanged();

};

#endif
