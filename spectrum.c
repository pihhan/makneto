/* Nalezeno na http://www.gtkforums.com/about3477.html&highlight= */

/* 
 * Spectrum analyser using GStreamer 
 * Source is a simple sinesrc 
 * Code based on the spectrum analyser sample in CVS, using OSS input 
 * See http://cvs.freedesktop.org/gstreamer/gst-plugins/gst/spectrum/demo-osssrc.c?rev=1.9&content-type=text%2Fplain 
 */ 

/* 
 * Expected result: one stable peak around 440Hz 
 * Result: something very fuzzy 
 */ 

/*gcc -Wall -g test.c -o test `pkg-config gstreamer-0.10 glib-2.0 gtk+-2.0 --cflags --libs`*/ 

#include <gst/gst.h> 
#include <gtk/gtk.h> 

/* For testing purposes, set the desired frequency of the sine wave */ 
/* Results do vary, but are not what I expect them to be */ 
/* Maybe it's just me */ 
#define SINEFREQ 440 

extern gboolean _gst_plugin_spew; 

gboolean idle_func (gpointer data); 

GtkWidget *drawingarea; 

float maxvalue;

#ifdef NO_ITERATE
gboolean gst_bin_iterate(gpointer data)
{
    return FALSE;
}
#endif

/* Draw the spectrum in some simple GTK window */ 
void 
spectrum_chain (GstElement * sink, GstBuffer * buf, GstPad * pad, 
    gpointer unused) 
{ 
  gint i; 
  guchar *data = buf->data; 
  GdkRectangle rect = { 0, 0, GST_BUFFER_SIZE (buf), 25 }; 

  gdk_window_begin_paint_rect (drawingarea->window, &rect); 
  gdk_draw_rectangle (drawingarea->window, drawingarea->style->black_gc, 
      TRUE, 0, 0, GST_BUFFER_SIZE (buf), 25); 
  for (i = 0; i < GST_BUFFER_SIZE (buf); i++) { 
    gdk_draw_rectangle (drawingarea->window, drawingarea->style->white_gc, 
        TRUE, i, 32 - data[i], 1, data[i]); 
  } 
  gdk_window_end_paint (drawingarea->window); 
} 

gboolean 
spectrum_message( GstBus *bus, GstMessage *message, gpointer data)
{
    if (message->type == GST_MESSAGE_ELEMENT) {
        const GstStructure *s = gst_message_get_structure(message);
        const gchar *name = gst_structure_get_name(s);

        if (!strcmp(name, "spectrum")) {
            const GValue *magnitudes;

            const int height = 25;
            GdkRectangle rect = { 0, 0, bands, height }; 
            gdk_draw_rectangle (drawingarea->window, drawingarea->style->black_gc, 
                TRUE, 0, 0, bands, height); 

            for (int i = 0; i< bands; ++i) {
                magnitudes = gst_value_list_get_value(magnitudes, i);
                float m = g_value_get_float(magnitudes);
                if (m>maxvalue)
                    maxvalue = m;
                int column = m/maxvalue*height;
                if (column < 0) 
                    column = -column;
                gdk_draw_rectangle (drawingarea->window, drawingarea->style->white_gc, 
                TRUE, i, 1, 1, column); 

            }
            gdk_window_end_paint (drawingarea->window); 

        }
    }
}

int 
main (int argc, char *argv[]) 
{ 
  /* Create all our elements */ 
  GstElement *bin; 
  GstElement *src, *spectrum, *sink; 

  GtkWidget *appwindow; 

  gst_init (&argc, &argv); 
  gtk_init (&argc, &argv); 

  bin = gst_pipeline_new ("bin"); 

  src = gst_element_factory_make ("alsasrc", "src"); 
  if (!src) {
      g_error("No source element");
      return 1;
  }
  /* Use same buffer size as in the osssrc sample */ 
  //g_object_set(G_OBJECT(src), "samplesperbuffer", (gulong) 1024, NULL); 
  /* We can change this, for testing purpose */ 
  //g_object_set(G_OBJECT(src), "freq", (double) SINEFREQ, NULL); 
  spectrum = gst_element_factory_make ("spectrum", "spectrum"); 
  if (!spectrum) {
      g_error("No spectrum element");
      return 1;
  }
  //g_object_set (G_OBJECT (spectrum), "width", 256, NULL); 
  sink = gst_element_factory_make ("fakesink", "sink"); 
  g_object_set (G_OBJECT (sink), "signal-handoffs", TRUE, NULL); 
  g_signal_connect (sink, "handoff", G_CALLBACK (spectrum_chain), NULL); 

  gst_bin_add_many (GST_BIN (bin), src, spectrum, sink, NULL); 
  gst_element_link_many (src, spectrum, sink, NULL); 

  appwindow = gtk_window_new (GTK_WINDOW_TOPLEVEL); 
  if (!appwindow) {
      g_error("No app window");
      return 1;
  }
  drawingarea = gtk_drawing_area_new (); 
  if (!drawingarea) {
      g_error("No drawing area");
  }
  gtk_drawing_area_size (GTK_DRAWING_AREA (drawingarea), 512, 32); 
  gtk_container_add (GTK_CONTAINER (appwindow), drawingarea); 
  gtk_widget_show_all (appwindow); 

  gst_element_set_state (GST_ELEMENT (bin), GST_STATE_PLAYING); 

  g_idle_add (idle_func, bin); 

  gtk_main (); 

  return 0; 
} 


gboolean 
idle_func (gpointer data) 
{ 
  return gst_bin_iterate (data); 
}
