
#ifndef FRAME_SIZE_H
#define FRAME_SIZE_H

#include <list>
#include <string>

struct FrameSize;

typedef std::list<FrameSize>    FrameSizeList;

/** @brief Miniclass for storage and printing of size. */
struct FrameSize
{
    public:
    FrameSize();
    FrameSize(unsigned int width, unsigned int height);

    unsigned int width;
    unsigned int height;

    std::string toString() const;
    bool operator==(const FrameSize &size) const;

    static std::string toString(const FrameSizeList &list);
    static bool contains(const FrameSizeList &list, const FrameSize &size);
    static void appendIfUnique(FrameSizeList &list, const FrameSize &size);
};


#endif
