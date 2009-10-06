
#ifndef CODECS_H
#define CODECS_H

#include <gst/gst.h>
#include <gst/farsight.h>

class CodecSet
{
    public:
    CodecSet();

    GList *codecsList();
    GList *videoCodecs();

    protected:
    /** @brief Add codec to supported list. */
    void addCodec(int id, const std::string &name, unsigned int rate);

    void addVideoCodec(int id, const std::string &name, unsigned int rate=90000);

    private:
    GList *m_list;
    GList *m_videolist;
    GError *m_lasterror;
};

#endif

