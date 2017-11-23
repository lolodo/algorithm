// gcc --std=c99 -Wall $(pkg-config --cflags gstreamer-1.0) -o gst gst.c $(pkg-config --libs gstreamer-1.0) -lgstapp-1.0

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

#include <stdint.h>

int want = 1;

uint32_t b_white[1280*800];
uint32_t b_black[1280*800];

static void prepare_buffer(GstAppSrc* appsrc) {

    static gboolean white = FALSE;
    static GstClockTime timestamp = 0;
    GstBuffer *buffer;
    guint size;
    GstFlowReturn ret;

    //if (!want) return;
    //want = 0;

    size = 1280 * 800 * 4;
    for (int i = 0; i < 1280*800; i++) { b_black[i] = 0x88884322; b_white[i] = 0xFFFFFFFF; }

    buffer = gst_buffer_new_wrapped_full( 0, (gpointer)(white?b_white:b_black), size, 0, size, NULL, NULL );

    white = !white;

    GST_BUFFER_PTS (buffer) = timestamp;
    GST_BUFFER_DURATION (buffer) = gst_util_uint64_scale_int (1, GST_SECOND, 4);

    timestamp += GST_BUFFER_DURATION (buffer);

    ret = gst_app_src_push_buffer(appsrc, buffer);

    if (ret != GST_FLOW_OK) {
        /* something wrong, stop pushing */
        // g_main_loop_quit (loop);
    }
}

static gboolean
bus_message (GstBus * bus, GstMessage * message,  GMainLoop *loop)
{
  GST_DEBUG ("got message %s",
      gst_message_type_get_name (GST_MESSAGE_TYPE (message)));

  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR: {
        GError *err = NULL;
        gchar *dbg_info = NULL;

        gst_message_parse_error (message, &err, &dbg_info);
        g_printerr ("ERROR from element %s: %s\n",
            GST_OBJECT_NAME (message->src), err->message);
        g_printerr ("Debugging info: %s\n", (dbg_info) ? dbg_info : "none");
        g_error_free (err);
        g_free (dbg_info);
        g_main_loop_quit (loop);
        break;
    }
    case GST_MESSAGE_EOS:
      g_main_loop_quit (loop);
      break;
    default:
      break;
  }
  return TRUE;
}

static void cb_need_data (GstElement *appsrc, guint unused_size, gpointer user_data) {
    prepare_buffer((GstAppSrc*)appsrc);
    //want = 1;
}

gint main (gint argc, gchar *argv[]) {


    GMainLoop *loop;
    GstBus *bus;
    GError *error = NULL;

    GstElement *pipeline, *appsrc, *conv, *videosink;

    for (int i = 0; i < 1280*800; i++) { b_black[i] = 0x88884322; b_white[i] = 0xFFFFFFFF; }

    /* init GStreamer */
    gst_init (&argc, &argv);
    loop = g_main_loop_new (NULL, TRUE);

    /* setup pipeline */
    //pipeline = gst_pipeline_new ("pipeline");
    pipeline = gst_parse_launch ("appsrc name=myappsrc ! videoconvert ! jpegenc !  tcpserversink port=8554 host=0.0.0.0", &error);
    //pipeline = gst_parse_launch ("appsrc name=myappsrc ! videoconvert ! jpegenc !  tcpserversink port=8554 host=127.0.0.1", &error);
    //pipeline = gst_parse_launch ("appsrc name=myappsrc !  rtpjpegpay ! udpsink host=127.0.0.1 port=8554", &error);
    //pipeline = gst_parse_launch ("appsrc name=myappsrc ! videoconvert ! video/x-raw, format=I420, width=1280,height=800 ! jpegenc ! rtpjpegpay ! udpsink host=127.0.0.1 port=8554", &error);
    //appsrc = gst_element_factory_make ("appsrc", "source");
    appsrc = GST_APP_SRC( gst_bin_get_by_name (GST_BIN(pipeline), "myappsrc") );
    //conv = gst_element_factory_make ("videoconvert", "conv");
    //videosink = gst_element_factory_make ("xvimagesink", "videosink");
    //
    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    g_assert(bus);

    /* add watch for messages */
    gst_bus_add_watch (bus, (GstBusFunc) bus_message, loop);

    /* setup */
    g_object_set (G_OBJECT (appsrc), "caps",
            gst_caps_new_simple ("video/x-raw",
                "format", G_TYPE_STRING, "ARGB",
                "width", G_TYPE_INT, 1280,
                "height", G_TYPE_INT, 800,
                "framerate", GST_TYPE_FRACTION, 0, 1,
                NULL), NULL);
    //gst_bin_add_many (GST_BIN (pipeline), appsrc, conv, videosink, NULL);
    //gst_element_link_many (appsrc, conv, videosink, NULL);

    /* setup appsrc */
    g_object_set (G_OBJECT (appsrc),
            "stream-type", 0, // GST_APP_STREAM_TYPE_STREAM
            "format", GST_FORMAT_TIME,
            "is-live", TRUE,
            NULL);
    g_signal_connect (appsrc, "need-data", G_CALLBACK (cb_need_data), NULL);
    g_signal_connect (appsrc, "seek-data", G_CALLBACK (cb_need_data), NULL);

    /* play */
    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    /*
    while (1) {
        usleep(100000);
        prepare_buffer((GstAppSrc*)appsrc);
        g_main_context_iteration(g_main_context_default(), FALSE);
    }
    */
    g_main_loop_run (loop);

    /* clean up */
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (bus);
    g_main_loop_unref (loop);
    gst_object_unref (GST_OBJECT (pipeline));

    return 0;
}
