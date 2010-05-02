
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdint.h>

#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <gst/interfaces/propertyprobe.h>
#include "gsttest.h"

GstTest::GstTest(bool test) 
    : pipeline(0),source(0),parser(0),decoder(0),convert(0),sink(0),
      level(0),volume(0), audiosrc(0), audiosink(0),
    playing(false),xwindowid(0)
{
    if (test)
    createVideoPipeline();
    else createPipeline();
}

void GstTest::createAudioPipeline()
{
    audiosrc = gst_element_factory_make("pulsesrc", "audiosrc");
    level = gst_element_factory_make("level", NULL);
    volume = gst_element_factory_make("volume", NULL);
    //audiosink = gst_element_factory_make("fakesink", NULL);
    GError *err = NULL;
    audiosink = gst_parse_bin_from_description("tee ! fakesink", TRUE, &err);

    if (!audiosrc || !level || !volume || !audiosink) {
        g_error("Audio creation failed.");
        return;
    }

    gst_bin_add_many(GST_BIN(pipeline), audiosrc, level, volume, audiosink, NULL);

    // audio part
    bool success = true;
    success = success && gst_element_link(audiosrc, level);
    success = success && gst_element_link(level, volume);
    success = success && gst_element_link(volume, audiosink);

    uint64_t    interval = 250000000ULL;
    g_object_set(GST_OBJECT(level), "interval", interval, NULL);

    if (success)
        printf("Audio part creation successful.\n");
    else
        g_error("Audio part linking failed.");
}

void GstTest::createPipeline()
{
    pipeline = gst_pipeline_new("gst-test");
    source = gst_element_factory_make("filesrc", "file-source");
    parser = gst_element_factory_make("oggdemux", "ogg-parser");
    decoder = gst_element_factory_make("vorbisdec", "vorbis-decoder");
    decoder2 = gst_element_factory_make("speexdec", "speex-decoder");
    convert = gst_element_factory_make("audioconvert", "converter");
    sink = gst_element_factory_make("autoaudiosink", "audio-output");

    if(!pipeline || !source || !parser || !decoder || !convert || !sink) {
        printf("Chyba inicializace gstreameru\n");
        return; // TODO: reportovat!
    }

    bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus, bus_call, this);

    gst_bin_add_many(GST_BIN(pipeline), source, parser, decoder, decoder2, 
            convert, sink, level, volume, NULL);
    gst_element_link(source, parser);
    //gst_element_link(decoder, convert);
    gst_element_link(convert, sink);

    createAudioPipeline();

    g_object_set(G_OBJECT(sink), "force-aspect-ratio", (gboolean) TRUE, NULL);

    g_signal_connect(parser, "pad-added", G_CALLBACK(pad_watch), this);

}

void GstTest::createTestPipeline()
{
    pipeline = gst_pipeline_new("gst-test");
    source = gst_element_factory_make("audiotestsrc", "beep-source");
    convert = gst_element_factory_make("audioconvert", "converter");
    sink = gst_element_factory_make("autoaudiosink", "audio-output");

    if(!pipeline || !source || !convert || !sink) {
        printf("Chyba inicializace gstreameru\n");
        return; // TODO: reportovat!
    }

    bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus, bus_call, this);

    gst_bin_add_many(GST_BIN(pipeline), source, convert, sink, NULL);
    gst_element_link(source, convert);
    gst_element_link(convert, sink);

}

void GstTest::createVideoPipeline()
{
    pipeline = gst_pipeline_new("gst-video-test");
    //source = gst_element_factory_make("videotestsrc", "video-source");
    source = gst_element_factory_make("v4l2src", "video-source");
    //parser = gst_element_factory_make("ffmpegcolorspace", "colorspace");
    parser = gst_parse_bin_from_description("ffmpegcolorspace ! videoscale", true, NULL);
    convert = gst_element_factory_make("videoscale", "scaler");
    sink = gst_element_factory_make("xvimagesink", "video-output");

    gst_x_overlay_prepare_xwindow_id(GST_X_OVERLAY(sink));

    if(!pipeline || !source || !convert || !sink) {
        printf("Chyba inicializace gstreameru\n");
        return; // TODO: reportovat!
    }

    bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus, bus_call, this);

    gst_bin_add_many(GST_BIN(pipeline), source, convert, sink, NULL);
    gst_bin_add(GST_BIN(pipeline), parser);
    gst_element_link(source, parser);
    gst_element_link(parser, convert);
//    gst_element_link(source, convert);
    gst_element_link(convert, sink);

    createAudioPipeline();

}

void GstTest::configureOverlay()
{
    printf("Configuring overlay to id %ld\n", xwindowid);
    //GST_X_OVERLAY(sink)->set_xwindow_id(GST_X_OVERLAY(sink), xwindowid);
    gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(sink), xwindowid);
}

gboolean GstTest::bus_call(GstBus * /*bus*/, GstMessage *msg, gpointer data)
{
    GstTest *test = (GstTest *) data;

    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_EOS:
            g_print("Konec souboru");
            if (test != NULL)
                test->stopped();
            // konec souboru!
            break;
        case GST_MESSAGE_ERROR:
            test->handleError(msg);
            break;
        case GST_MESSAGE_ELEMENT:
            if (test && gst_structure_has_name(msg->structure, "level"))
                test->watchers.propagateLevelMessage(msg);
            printf("Gst Element Message type %d\n", GST_MESSAGE_TYPE(msg));
            break;
        case GST_MESSAGE_STATE_CHANGED:
            printf("Gst State Message type %d/%s from %s\n", 
                    GST_MESSAGE_TYPE(msg), GST_MESSAGE_TYPE_NAME(msg),
                    gst_object_get_name(msg->src));
            break;
        case GST_MESSAGE_STATE_DIRTY:
            printf("Gst State Dirty Message type %d\n", GST_MESSAGE_TYPE(msg));
            break;
            
        default:
            printf("Gst Bus Message type %d/%s from %s\n", 
                    GST_MESSAGE_TYPE(msg), GST_MESSAGE_TYPE_NAME(msg),
                    gst_object_get_name(GST_OBJECT(msg->src)));
            break;
    }
    return true;
}


void GstTest::pad_watch(GstElement * /*element*/, GstPad *pad, gpointer data)
{
    GstTest *test = (GstTest *) data;

    printf("pad watch triggered\n");
    if (test) {
        test->connectDecoder(pad);
    }
}

/** \brief Pripoj nove objeveny stream do dekoderu */
void GstTest::connectDecoder(GstPad *pad)
{
    GstPad *sinkpad = gst_element_get_pad(decoder, "sink");
    if (!sinkpad) {
        printf("Sinkpad NULL!\n");
        return;
    }
    printf("Pad %s has caps: %s\n", gst_pad_get_name(sinkpad), 
            gst_caps_to_string(gst_pad_get_caps(sinkpad)));
    printf("Connecting caps %s to decoder %s\n", 
            gst_caps_to_string(gst_pad_get_caps(pad)), gst_object_get_name(GST_OBJECT(decoder)));
    GstPadLinkReturn ret = gst_pad_link(pad, sinkpad);
    if (ret != GST_PAD_LINK_OK) {
        printf("Dynamic pad link failed: %d\n", ret);
        gst_object_unref(sinkpad);
        sinkpad = gst_element_get_pad(decoder2, "sink");

        ret = gst_pad_link(pad, sinkpad);
        if (ret != GST_PAD_LINK_OK) {
            printf("Second pad link failed also.\n");
        } else {
            printf("Second pad link succeded\n");
            gst_element_link(decoder2, convert);
        }
    } else {
        printf("Pad succeeded\n");
        gst_element_link(decoder, convert);
    }
    gst_object_unref(sinkpad);
}

void GstTest::handleError(GstMessage *msg)
{
    GError *err;
    gchar *debug;

    gst_message_parse_error(msg, &err, &debug);
    g_print("Chyba %s [%s]\n", err->message, debug);
    g_free(debug);
    g_error_free(err);
}


void GstTest::initialize(int argc, char *argv[])
{
    gst_init(&argc, &argv);
}

void GstTest::play()
{
    printf("Setting to play\n");
    gst_element_set_state((pipeline), GST_STATE_PLAYING);
    configureOverlay();
    playing = true;
}

void GstTest::pause()
{
    printf("Setting to pause\n");
    playing = false;
    gst_element_set_state((pipeline), GST_STATE_PAUSED);
}

void GstTest::stop()
{
    printf("Setting to stop\n");
    playing = false;
    gst_element_set_state((pipeline), GST_STATE_NULL);
}

void GstTest::togglePlay()
{
    if (playing) {
        stop();
    } else {
        play();
    }
}

void GstTest::setLocation(const char *path)
{
    g_object_set(G_OBJECT(source), "location", path, NULL);
    printf("Location: %s\n", path);
}

void GstTest::stopped()
{
    printf("Pipeline stopped.\n");
}

void GstTest::printSourcePads()
{
    GstIterator *it = gst_element_iterate_pads(source);
    GstPad *p = NULL;
    bool terminate = false;
    do {
        switch (gst_iterator_next(it, ((void **) &p))) {
            case GST_ITERATOR_OK:
                {
                    std::cout << "Pad " << gst_pad_get_name(p) << std::endl;
                    GstCaps * c = gst_pad_get_caps(p);
                    if (c) {
                        std::cout << "caps: " << gst_caps_to_string(c) << std::endl;
                        gst_caps_unref(c);
                    } else 
                        std::cout << "NULL caps" << std::endl;

                    gst_object_unref(p);
                }
                break;
            case GST_ITERATOR_DONE:
                terminate = true;
                break;
            case GST_ITERATOR_RESYNC:
                gst_iterator_resync(it);
                break;
            case GST_ITERATOR_ERROR:
                terminate = true;
                break;
        }
    } while (!terminate);

    std::cout << std::endl;

    if (level) {
        gboolean message = false;
        uint64_t period = 0;
        g_object_get(G_OBJECT(level), "message", &message, "interval", &period, NULL);
        std::cout << "Level is present with state: " << GST_STATE(level) 
            << " message: " << message 
            << " interval: " << period << std::endl;
    }
}

/** @brief Get supported resolutions. */
std::vector<int> GstTest::getResolutions(GstCaps *caps)
{
    guint size = gst_caps_get_size(caps);
    std::vector<int> resolutions;

    for (guint i = 0; i < size; i++) {
        GstStructure *s = gst_caps_get_structure(caps, i);
        const GValue *h = gst_structure_get_value(s, "height");
        const GValue *w = gst_structure_get_value(s, "width");
        if (!h || !w) 
            continue;
        if (G_VALUE_TYPE(h) == G_TYPE_INT) {
            guint height = g_value_get_int(h);
            guint width = g_value_get_int(w);
            if (resolutions.size()+1 < width) {
                resolutions.resize(width+1, 0);
            }
            resolutions[width] = height;
        } else if (G_VALUE_TYPE(h) == G_TYPE_UINT) {
            guint height = g_value_get_uint(h);
            guint width = g_value_get_uint(w);
            if (resolutions.size()+1 < width) {
                resolutions.resize(width+1, 0);
            }
            resolutions[width] = height;
        } else {
            std::cerr << "GValue for height has unsupported type " 
                << G_VALUE_TYPE(h) << std::endl;
        }
    } // for
    return resolutions;
}

std::vector<int> GstTest::getResolutions()
{
    std::vector<int> resolutions;
    GstIterator *it = gst_element_iterate_pads(source);
    GstPad *p = NULL;
    bool terminate = false;
    do {
        switch (gst_iterator_next(it, ((void **) &p))) {
            case GST_ITERATOR_OK:
                {
                    GstCaps * c = gst_pad_get_caps(p);
                    if (c) {
                        resolutions = getResolutions(c);
                        gst_caps_unref(c);
                    } else 
                        std::cout << "NULL caps" << std::endl;

                    gst_object_unref(p);
                }
                break;
            case GST_ITERATOR_DONE:
                terminate = true;
                break;
            case GST_ITERATOR_RESYNC:
                gst_iterator_resync(it);
                break;
            case GST_ITERATOR_ERROR:
                terminate = true;
                break;
        }
    } while (!terminate);

    return resolutions;
}

void GstTest::setResolution(int w, int h)
{
    GstPad *pad = gst_element_get_pad(source, "src");
    if (!pad) {
        std::cerr << "Null source pad" << std::endl;
        return;
    }

    GstCaps *resolution = gst_caps_new_simple("video/x-raw-yuv",
        "width", G_TYPE_INT, w,
        "height", G_TYPE_INT, h,
        NULL);
    if (!resolution) {
        std::cerr << "Nezdarila se tvorba caps!" << std::endl;
        return;
    }
#if 0
    GstCaps *caps = gst_pad_get_allowed_caps(pad);
    if (!caps) {
        std::cerr << "Null caps" << std::endl;
        return;
    }
    GstCaps *preintersect = gst_caps_intersect(caps, resolution);
    GstCaps *intersect = NULL;

    bool success = false;
    if (!gst_caps_is_empty(preintersect))
    for (unsigned int i = 0; i < gst_caps_get_size(caps) && !success; i++) {
        intersect = gst_caps_copy_nth(preintersect, i);
        if (gst_caps_is_empty(intersect)) {
            gst_caps_unref(intersect);
            continue;
        }
        bool fixed = gst_caps_is_fixed(intersect);
        std::cerr << "Setting intersected caps: " 
            << gst_caps_to_string(intersect)
            << "; fixed: " 
            << fixed << std::endl;
        if (!fixed) {
            gst_caps_unref(intersect);
            continue;
        }

        if (!gst_pad_set_caps(pad, intersect)) {
            std::cerr << "Setting intersected caps failed!" << std::endl;
        } else {
            success  = true;
        }

        gst_caps_unref(intersect);
    }

    gst_caps_unref(preintersect);
    gst_object_unref(GST_OBJECT(pad));
    gst_caps_unref(caps);
#else
    GstCaps *caps = gst_pad_get_caps(pad);
    if (!caps) {
        std::cerr << "Null caps" << std::endl;
        return;
    }
    GstCaps *preintersect = gst_caps_intersect(caps, resolution);
    if (pad) {
        std::cout << "Pad is linked: " << gst_pad_is_linked(pad) << std::endl;
    }
    gst_object_unref(GST_OBJECT(pad));
    gst_element_unlink(source, parser);
    pad = gst_element_get_pad(source, "src");
    if (pad) {
        if (gst_pad_is_linked(pad)) {
            GstPad *peer = gst_pad_get_peer(pad);
            bool unlinked = gst_pad_unlink(pad, peer);
            gst_object_unref(GST_OBJECT(peer));
            std::cout << "Pad unlinked: " << unlinked << std::endl;
        }
        std::cout << "Pad is linked: " << gst_pad_is_linked(pad) << std::endl;
        gst_object_unref(GST_OBJECT(pad));
    }
    if (preintersect) {
        std::cout << "Preintersect caps are present." << std::endl;
    }
    if (!gst_element_link_filtered(source, parser, preintersect)) {
        std::cerr << "Re-Linking source with resolution failed." << std::endl;
       
#if 0
        GstPad *srcpad = gst_element_get_pad(source, "src");
        GstPad *sinkpad = gst_element_get_pad(parser, "sink");
        if (!gst_pad_link_filtered(srcpd, sinkpad, preintersect)) {
            std::cerr << "Re-Linking source pad with resolution failed also." << std::endl;
        }
        gst_object_unref(GST_OBJECT(srcpad));
        gst_object_unref(GST_OBJECT(sinkpad));
#endif
    }
#endif

    gst_caps_unref(resolution);

}

int GstTest::negotiatedWidth()
{
    if (!sink)
        return -1;
    GstPad *pad = gst_element_get_pad(sink, "sink");
    if (!pad)
        return -1;
    GstCaps *caps = gst_pad_get_negotiated_caps(pad);
    if (caps) {
        for (unsigned int i = 0; i< gst_caps_get_size(caps); i++) {
            GstStructure *s = gst_caps_get_structure(caps, i);
            int size = -1;
            if (!gst_structure_get(s, "width", G_TYPE_INT, &size, NULL)) {
                std::cerr << "Failed to get width of negotiated caps." << std::endl;
            } else {
                gst_caps_unref(caps);
                return size;
            }
        }
        return -1;
    } else {
        return -1;
    }
}

int GstTest::negotiatedHeight()
{
    if (!sink)
        return -1;
    GstPad *pad = gst_element_get_pad(sink, "sink");
    if (!pad)
        return -1;
    GstCaps *caps = gst_pad_get_negotiated_caps(pad);
    if (caps) {
        unsigned int size = gst_caps_get_size(caps);
        if (size > 3)
            size = 3;
        for (unsigned int i = 0; i< size; i++) {
            GstStructure *s = gst_caps_get_structure(caps, i);
            int size = -1;
            if (!gst_structure_get(s, "height", G_TYPE_INT, &size, NULL)) {
                std::cerr << "Failed to get height of negotiated caps." << std::endl;
            } else {
                gst_caps_unref(caps);
                return size;
            }
        }
        return -1;
    } else {
        return -1;
    }
}

void GstTest::enumerateDevices(GstElement *source)
{
    // if (gst_element_implements_interface(source, GST_TYPE_PROPERTY_PROBE)) {
    if (GST_IS_PROPERTY_PROBE(source)) {
        GstPropertyProbe *probe = GST_PROPERTY_PROBE(source);
        const GList *list = gst_property_probe_get_properties(probe);
        for (const GList *i = (list); i != NULL; i = g_list_next(i)) {
            const GParamSpec *spec = (const GParamSpec *) i->data;
            std::cout << "Property name=" << spec->name
                << " has value type=" << g_type_name(spec->value_type);
            if (gst_property_probe_needs_probe(probe, spec))
                std::cout << " needs probing, ";
            std::cout << std::endl;

            GValueArray *vals = gst_property_probe_probe_and_get_values(probe, spec);
            for (guint pos=0; pos < vals->n_values; pos++) {
                const GValue v = vals->values[pos];
                GValue strval;
                memset(&strval, 0, sizeof(strval));
                g_value_init(&strval, G_TYPE_STRING);
                g_value_transform(&v, &strval);

                std::cout << "Value " << G_VALUE_TYPE_NAME(&v) 
                    << "=" << g_value_get_string(&strval) << std::endl;
            }
        }
    }
}

