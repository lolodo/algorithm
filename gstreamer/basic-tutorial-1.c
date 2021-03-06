/*************************************************************************
    > File Name: basic-tutorial-1.c
    > Author: Alex
    > Mail: fangyuan.liu@nio.com 
    > Created Time: Fri 29 Sep 2017 03:50:38 PM CST
 ************************************************************************/
#include <gst/gst.h>

int main(int argc, char *argv[])
{
	GstElement *pipeline;
	GstBus *bus;
	GstMessage *msg;

	/* Initialize GStreamer */
	gst_init(&argc, &argv);

	/* Build the pipeline */
	pipeline = gst_parse_launch("playbin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm", NULL);

	/* Start playing */
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	/* Waiting until error or EOS */
	bus = gst_element_get_bus(pipeline);
	msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

	/* Free resources */
	if (msg != NULL) {
		gst_message_unref(msg);
	}

	gst_object_unref(msg);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);

	return 0;
}
