/*************************************************************************
    > File Name: basic-tutorial-3.c
    > Author: Alex
    > Mail: fangyuan.liu@nio.com 
    > Created Time: 2017年12月01日 星期五 01时29分22秒
 ************************************************************************/
#include <gst/gst.h>

/* Structure to contain all our information, so we can pass it to callbacks  */
typedef struct _CustomData {
	GstElement *pipeline;
	GstElement *source;
	GstElement *convert;
	GstElement *sink;
} CustomData;

/* Handler for the pad-added signal */

int main(int argc, char *argv[])
{
	CustomData data;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;
	gboolean terminate = FALSE;

	/* Initialize GStreamer */
	gst_init(&argc, &argv);

	/* Create the elements */
	data.source = gst_element_factory_make("uridecodebin", "source");
	data.convert = gst_element_factory_make("audioconvert", "convert");
	data.sink = gst_element_factory_make("autoaudiosink", "sink");

	/* Create the empty pipeline */
	data.pipeline = gst_pipeline_new("test-pipeline");

	if (!data.pipeline || !data.source || !data.convert || !data.sink) {
		g_printerr("Not all elements could be created.\n");
		return -1;
	}

	/* Build the pipeline. Note that we are NOT linking the source at this point. We will do it later. */
	gst_bin_add_many(GST_BIN(data.pipeline), data.source, data.convert, data.sink, NULL);
	if (!gst_element_link(data.convert, data.sink)) {
		g_printerr("Elements could not be linked.\n");
		gst_object_unref(data.pipeline);
		return -1;
	}

	/* Set the URI to play  */
	g_object_set(data.source, "uri", "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm", NULL);

	/* Connect to the pad-added signal */
	g_signal_connect(data.source, "pad-added", G_CALLBACK(pad_added_handler), &data);

	/* Start playing  */
	ret = gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
	if (ret == GST_STATE_CHANGE_FAILURE) {
		g_printerr("Unable to set the pipeline to the playing state.\n");
		gst_object_unref(data.pipeline);
		return -1;
	}


	/* Listen to the bus */
	bus = get_element_get_bus(data.pipeline);
	do {
		msg = get_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NON, GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS);


		/* Parse message */
		if (msg != NULL) {
			GError *err;
			gchar *debug_info;

		}
	}
