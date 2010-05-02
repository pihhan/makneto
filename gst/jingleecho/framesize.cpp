
#include <sstream>
#include "framesize.h"

FrameSize::FrameSize()
    : width(0), height(0)
{
}

FrameSize::FrameSize(unsigned int width, unsigned int height)
    : width(width), height(height)
{
}

bool FrameSize::operator==(const FrameSize &size) const
{
    return (width == size.width && height == size.height);
}

std::string FrameSize::toString() const 
{
    std::ostringstream o;
    o << width << "x" << height;
    return o.str();
}

std::string FrameSize::toString(const FrameSizeList &list)
{
    std::ostringstream o;
    bool first = true;
    for (FrameSizeList::const_iterator it=list.begin(); it!=list.end(); it++) {
        if (!first)
            o << ", ";
        o << it->toString();
        first = false;
    }
    return o.str();
}

bool FrameSize::contains(const FrameSizeList &list, const FrameSize &size)
{
    for (FrameSizeList::const_iterator it=list.begin(); it!=list.end(); it++) {
        if ((*it) == size)
            return true;
    }
    return false;
}

void FrameSize::appendIfUnique(FrameSizeList &list, const FrameSize &size)
{
    if (!contains(list,size))
        list.push_back(size);
}

