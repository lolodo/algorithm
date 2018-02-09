/*************************************************************************
    > File Name: get_streaming_video.c
    > Author: Alex
    > Mail: fangyuan.liu@nio.com 
    > Created Time: Thu 25 Jan 2018 11:47:28 AM CST
 ************************************************************************/

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <stdio.h>
#include <string.h>

GError *error = NULL;
GMainLoop *loop;
GstElement *appsrc, *rtph264depay, *avdec_h264, *pipeline_sink, *appsink, *pipeline, *app_sink;
GstBus *bus;
GstSample *sample;
GstBuffer *recv_buffer;
GstMapInfo map;

int buffer_size;
unsigned int *gst_buffer;

static void cb_error(GstBus *bus, GstMessage *msg, void *data) 
{
    GError *err;
    gchar *debug_info;

    /* Print error details on the screen */ 
    gst_message_parse_error (msg, &err, &debug_info); 
    g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message); 
    g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
    g_clear_error (&err);
    g_free(debug_info);

    g_main_loop_quit(loop);
}

int free_pull_buffer()
{
    gst_buffer_unmap(recv_buffer, &map);
    gst_sample_unref(sample);
}

int counter = 0;
static GstFlowReturn cb_new_sample(GstElement *sink, int *size)
{
    g_signal_emit_by_name(sink, "pull-sample", &sample);
    if (sample != NULL) {
        recv_buffer = gst_sample_get_buffer(sample);
        gst_buffer_map(recv_buffer, &map, GST_MAP_READ);
        if (map.data != NULL) {
            gst_buffer = (unsigned int *)map.data;
            *size = map.size;
            counter++;
            printf("size:%-6lu ", map.size);
            printf("0x%08x 0x%08x 0x%08x 0x%08x ", *(gst_buffer), *(gst_buffer + 1), *(gst_buffer + 2), *(gst_buffer + 3));
#if 0
            printf("0x%08x 0x%08x 0x%08x 0x%08x\n", *(gst_buffer + 4), *(gst_buffer + 5), *(gst_buffer + 6), *(gst_buffer + 7));
#endif
            printf("end:");
#if 0
            printf("0x%08x 0x%08x 0x%08x 0x%08x\n", *(gst_buffer + (map.size / 4) - 7), *(gst_buffer + (map.size / 4) - 6), *(gst_buffer + (map.size / 4) - 5), *(gst_buffer + (map.size / 4) - 4));
#endif
            printf("0x%08x 0x%08x 0x%08x 0x%08x\n", *(gst_buffer + (map.size / 4) - 3), *(gst_buffer + (map.size / 4) - 2), *(gst_buffer + (map.size / 4) - 1), *(gst_buffer + (map.size / 4) - 0));
            free_pull_buffer();
            return GST_FLOW_OK;
        } else {
            *size = -1;
            gst_buffer_unmap(recv_buffer, &map);
            return GST_FLOW_NOT_NEGOTIATED;
        }
    } else {
        *size = -1;
        gst_buffer_unmap(recv_buffer, &map);
        return GST_FLOW_ERROR;
    }

}


int camera_sink_init()
{
    GstCaps *appcaps;

    /* Create the elements */
    appsrc = gst_element_factory_make("udpsrc", "gstappsrc");
    rtph264depay = gst_element_factory_make("rtph264depay", "gstdepay");
    avdec_h264 = gst_element_factory_make("avdec_h264", "gsth264");
    app_sink = gst_element_factory_make("appsink", "app_sink");

    /* Create the empty pipline */
    pipeline = gst_pipeline_new("gstpipeline");

    if (!appsrc || ! rtph264depay || !avdec_h264 || !pipeline || !app_sink) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    /* Configure appsrc */
    appcaps = gst_caps_new_simple ("application/x-rtp","clock-rate", G_TYPE_INT, 90000, "payload", G_TYPE_INT, 96, NULL);
    g_object_set(G_OBJECT(appsrc), "caps", appcaps, NULL);
    g_object_set(G_OBJECT(appsrc), "port", 8888, "address", 0, NULL);
    gst_caps_unref(appcaps);

    /* Configure appsink */
    g_object_set(app_sink, "emit-signals", TRUE, NULL);
    g_signal_connect(app_sink, "new-sample", G_CALLBACK(cb_new_sample), &buffer_size);

    /* Link all elements that can be automatically linked because they have "Always" pads */
    //gst_bin_add_many(GST_BIN(pipeline), app_sink, avdec_h264, rtph264depay, appsrc, NULL);
    gst_bin_add_many(GST_BIN(pipeline), app_sink, rtph264depay, appsrc, NULL);
    //if (gst_element_link_many(appsrc, rtph264depay, avdec_h264, app_sink, NULL) != TRUE) {
    if (gst_element_link_many(appsrc, rtph264depay, app_sink, NULL) != TRUE) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }
    
    /* Instruct the bus to emit signals for each received message, and connect to the interesting signals */
    bus = gst_element_get_bus(pipeline);
    gst_bus_add_signal_watch(bus);
    g_signal_connect(G_OBJECT(bus), "message::error", (GCallback)cb_error, NULL);
    gst_object_unref(bus);

    /* play */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    
    loop = g_main_loop_new(NULL, TRUE);
    g_main_loop_run(loop);

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));
}

int main(int argc, char *argv[])
{
    gst_init(&argc, &argv);
    camera_sink_init();
}

