#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>

#include <stdint.h>
#include <stdio.h>

#include "avm_common.h"

GError *error = NULL;

GMainLoop *loop;
GstBus *bus;
GstElement *pipeline, *appsrc, *conv, *videosink;

GMainLoop *loop_sink;
GstBus *bus_sink;
GstElement *pipeline_sink, *appsink;

GstSample *sample;
GstMapInfo map; 
GstBuffer *recv_buff;

void buffer_destroy_notify (gpointer data)
{
    printf("[%s]%d data:%x\n", __func__, __LINE__, data);
}

int prepare_buffer(char *buff_send, uint32_t size) 
{

    static gboolean white = FALSE;
    static GstClockTime timestamp = 0;
    GstBuffer *buffer;
    GstFlowReturn ret;


    if (buff_send == NULL)
        return -1;

    //printf("[%s]%d data:%x\n", __func__, __LINE__, buff_send);

    buffer = gst_buffer_new_wrapped_full( 0, buff_send, size, 0, size, (gpointer)buff_send, buffer_destroy_notify);

    ret = gst_app_src_push_buffer(appsrc, buffer);

    if (ret != GST_FLOW_OK) {
        /* something wrong, stop pushing */
        // g_main_loop_quit (loop);
    }

    return ret;

}

int free_pull_buffer()
{
    gst_buffer_unmap(recv_buff, &map);
    gst_sample_unref(sample);
}

static gboolean bus_message (GstBus * bus, GstMessage * message,  GMainLoop *loop)
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

int pull_buffer(char **buff)
{
    sample = gst_app_sink_pull_sample((GstAppSink*)appsink);
    if (sample != NULL) {
        recv_buff = gst_sample_get_buffer (sample);
        gst_buffer_map (recv_buff, &map, GST_MAP_READ); 
        if (map.data != NULL) {
            *buff = (char *)map.data;
            return map.size;
        } else {
            return 0;
        }
    } else 
        return 0;

}

static void cb_new_sample (GstElement *appsink, guint unused_size, gpointer user_data) 
{
    //gst_app_sink_pull_sample((GstAppSrc*)appsink);
    printf("[%s]%d size:%d\n", __func__, __LINE__, unused_size);
}

static void cb_need_data (GstElement *appsrc, guint unused_size, gpointer user_data) 
{
    //prepare_buffer((GstAppSrc*)appsrc, NULL);
    printf("[%s]%d size:%d\n", __func__, __LINE__, unused_size);
}

int libgst_init(int argc, char *argv[]) 
{
    gst_init (&argc, &argv);
}

int gst_appsrc_init() 
{
    char gst_string[200];
    loop = g_main_loop_new (NULL, TRUE);

    /* setup pipeline */
    //pipeline = gst_parse_launch ("appsrc name=myappsrc ! videoconvert ! jpegenc !  tcpserversink port=8554 host=0.0.0.0", &error);
    //pipeline = gst_parse_launch ("appsrc name=myappsrc ! jpegenc ! rtpjpegpay ! udpsink host=127.0.0.1 port=8554", &error);
    //snprintf(gst_string, 200, "appsrc name=myappsrc ! videoconvert ! jpegenc ! rtpjpegpay ! udpsink host=%s port=%d", dest_address, dest_port);
    //snprintf(gst_string, 200, "appsrc name=myappsrc ! videoconvert  ! video/x-raw, format=I420, width=1280,height=800 ! jpegenc ! tcpclientsink host=%s port=%d", dest_address, dest_port);
    //snprintf(gst_string, 200, "appsrc name=myappsrc ! videoconvert  ! video/x-raw, format=I420, width=1280,height=800 ! jpegenc ! rtpjpegpay ! udpsink host=%s port=%d", dest_address, dest_port);
    snprintf(gst_string, 200, "appsrc name=myappsrc ! jpegenc ! rtpjpegpay ! udpsink host=%s port=%d", dest_address, dest_port);
    pipeline = gst_parse_launch (gst_string, &error);
    /*
    pipeline = gst_parse_launch ("appsrc name=myappsrc ! videoconvert ! video/x-raw, format=I420, width=1280,height=720 ! jpegenc ! rtpjpegpay ! udpsink host=127.0.0.1 port=8554", &error);
    */
    appsrc = GST_APP_SRC( gst_bin_get_by_name (GST_BIN(pipeline), "myappsrc") );

    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    g_assert(bus);

    /* add watch for messages */
    gst_bus_add_watch (bus, (GstBusFunc) bus_message, loop);

    /* setup */
    g_object_set (G_OBJECT (appsrc), "caps",
            gst_caps_new_simple ("video/x-raw",
                "format", G_TYPE_STRING, dest_format,
                "width", G_TYPE_INT, dest_width,
                "height", G_TYPE_INT, dest_height,
                "framerate", GST_TYPE_FRACTION, 0, 1,
                NULL), NULL);

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

    return 0;
}

int gst_appsrc_fini()
{
    /* clean up */
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (bus);
    g_main_loop_unref (loop);
    gst_object_unref (GST_OBJECT (pipeline));

    return 0;
}

int gst_appsink_init() 
{
    char gst_string[200];
    loop_sink = g_main_loop_new (NULL, TRUE);

    /* setup pipeline */
    snprintf(gst_string, 200, "v4l2src device=/dev/video%d ! appsink name=myappsink", v4l2_device);
    pipeline_sink = gst_parse_launch (gst_string, &error);
    appsink = GST_APP_SINK( gst_bin_get_by_name (GST_BIN(pipeline_sink), "myappsink") );

    bus_sink = gst_pipeline_get_bus (GST_PIPELINE (pipeline_sink));
    g_assert(bus_sink);

    /* add watch for messages */
    gst_bus_add_watch (bus_sink, (GstBusFunc) bus_message, loop_sink);

    /* setup */
    g_object_set (G_OBJECT (appsink), "caps",
            gst_caps_new_simple ("video/x-raw",
                "format", G_TYPE_STRING, src_format,
                "width", G_TYPE_INT, src_width,
                "height", G_TYPE_INT, src_height,
                NULL), NULL);
    gst_app_sink_set_max_buffers(appsink, 10);
    g_object_set (G_OBJECT (appsink),
            "drop", TRUE, NULL);

    /* setup appsrc */
    g_signal_connect (appsink, "new-sample", G_CALLBACK (cb_new_sample), NULL);

    /* play */
    gst_element_set_state (pipeline_sink, GST_STATE_PLAYING);

    return 0;
}

int gst_appsink_fini()
{
    /* clean up */
    gst_element_set_state (pipeline_sink, GST_STATE_NULL);
    gst_object_unref (bus_sink);
    g_main_loop_unref (loop_sink);
    gst_object_unref (GST_OBJECT (pipeline_sink));

    return 0;
}
