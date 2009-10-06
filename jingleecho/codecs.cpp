
#include "codecs.h"

/**
@see http://www.iana.org/assignments/rtp-parameters
*/

CodecSet::CodecSet()
    : m_list(NULL),m_videolist(NULL),m_lasterror(NULL)
{
    addCodec(96, "speex", 16000);
    addCodec(97, "speex", 8000);
    addCodec(18, "G729", 0);
    addCodec(11, "L16", 44100); // 10 je stereo se stejnymi parametry
    addCodec(103, "L16", 16000);
    addCodec(0, "PCMU", 16000);
    addCodec(8, "PCMA", 0);

    addCodec(3, "GSM", 8000);
    addCodec(4, "G723", 8000);
}

void CodecSet::addCodec(int id, const std::string &name, unsigned int rate)
{
    g_list_prepend(m_list, fs_codec_new(id, name.c_str(), FS_MEDIA_TYPE_AUDIO, rate));
}

void CodecSet::addVideoCodec(id, const std::string &name, unsigned int rate)
{
    g_list_prepend(m_videolist,
        fs_codec_new(id, name.c_str(), FS_MEDIA_TYPE_VIDEO, rate)
    );
}

