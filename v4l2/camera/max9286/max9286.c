/*
 * Driver for MAXIM CMOS Image Sensor from MAXIM
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/videodev2.h>
#include <linux/module.h>
#include <linux/v4l2-mediabus.h>
#include <linux/media-bus-format.h>

#include <linux/clk.h>
#include <media/soc_camera.h>
#include <media/v4l2-clk.h>
#include <media/v4l2-subdev.h>

/*
 * About MAX resolution, cropping and binning:
 * This sensor supports it all, at least in the feature description.
 * Unfortunately, no combination of appropriate registers settings could make
 * the chip work the intended way. As it works with predefined register lists,
 * some undocumented registers are presumably changed there to achieve their
 * goals.
 * This driver currently only works for resolutions up to 720 lines with a
 * 1:1 scale. Hopefully these restrictions will be removed in the future.
 */
#define MAXIM_MAX_WIDTH	2592
#define MAXIM_MAX_HEIGHT	720
#define MAX9286_ADDR 0x90
#define MAX9286_ID 0x40
#define MAX_REG_LEN 2
#define MAX9271_INIT_ADDR 0x80
#define MAX9271_ALL_ADDR 0x80
#define MAX9271_CH0_ADDR 0x82
#define MAX9271_CH1_ADDR 0x84
#define MAX9271_CH2_ADDR 0x86
#define MAX9271_CH3_ADDR 0x88
#define OV490_CH0_ADDR 0x62
#define OV490_INIT_ADDR 0x48
#define OV490_CH0_MAP_ADDR 0x60
#define OV490_CH1_MAP_ADDR 0x62
#define OV490_CH2_MAP_ADDR 0x64
#define OV490_CH3_MAP_ADDR 0x66
#define MAX9286_1CH_LANE 1
//#define CAB888_TEST_PATTERN


#define OV490_ID 0xB888
#define MAX9286_LINK_CONFIG_REG 0x00
#define MAX9286_ID_REG   0x1E
#define MAX9286_LOCK_REG 0x27
#define MAX9286_LINK_REG 0x49

/* minimum extra blanking */
#define BLANKING_EXTRA_WIDTH		500
#define BLANKING_EXTRA_HEIGHT		20

/*
 * the sensor's autoexposure is buggy when setting total_height low.
 * It tries to expose longer than 1 frame period without taking care of it
 * and this leads to weird output. So we set 1000 lines as minimum.
 */
#define BLANKING_MIN_HEIGHT	1000
#define SENSOR_ID               (0x86)

#define max9286_info(fmt, args...)                \
		(void)pr_info("[max9286][info] %s %d: %s " fmt "\n",\
			__func__, __LINE__, dev_name(&client->dev), ##args)

#define max9286_err(fmt, args...)                \
		(void)pr_info("[max9286][error] %s %d: %s " fmt "\n",\
			__func__, __LINE__, dev_name(&client->dev), ##args)


struct reg_val_ops {
	u16 slave_addr;
	u8 reg[MAX_REG_LEN];
	u8 val;
	unsigned int reg_len;
	int (*i2c_ops)(struct i2c_client *client, u16 slave_addr,
		u8 *reg, unsigned int reg_len, u8 *val);
};

static int i2c_write(struct i2c_client *client, u16 slave_addr,
		u8 *reg, unsigned int reg_len, u8 *val);
static int i2c_read(struct i2c_client *client, u16 slave_addr,
		u8 *reg, unsigned int reg_len, u8 *val);
static int i2c_delay(struct i2c_client *client, u16 slave_addr,
		u8 *reg, unsigned int reg_len, u8 *value);
static int read_max9286_id(struct i2c_client *client, u8 *id_val);
static int max9286_write_array(struct i2c_client *client,
		struct reg_val_ops *cmd, unsigned long len);

/* Supported resolutions */
enum max9286_width {
	W_TESTWIDTH	= 720,
	W_WIDTH		= 736,
};

enum max9286_height {
	H_1CHANNEL	= 480,
	H_4CHANNEL	= 1920,
};

struct max9286_win_size {
	enum max9286_width width;
	enum max9286_height height;
};

static const struct max9286_win_size max9286_supported_win_sizes[] = {
	{ W_TESTWIDTH,	H_1CHANNEL },
	{ W_TESTWIDTH,	H_4CHANNEL },
	{ W_WIDTH,	H_1CHANNEL },
	{ W_WIDTH,	H_4CHANNEL },
};

static int is_testpattern;

struct sensor_addr {
	u16 ser_init_addr;
	u16 ser_last_addr;
	u16 isp_init_addr;
	u16 isp_map_addr;
};

#define CAB888_WIDTH 1280U
#define CAB888_HEIGHT 720U

static struct reg_val_ops MAX9286_CAB888_4v4_init_cmd[] = {
	{MAX9286_ADDR,      {0x0D, 0x00}, 0x03,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x3F, 0x00}, 0x4F,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x3B, 0x00}, 0x1E,               0x01, i2c_write},

	{MAX9286_ADDR,      {0x28, 0x00}, 0x00,               0x01, i2c_read},
	{MAX9286_ADDR,      {0x28, 0x00}, 0x00,               0x01, i2c_read},
	{MAX9286_ADDR,      {0x29, 0x00}, 0x00,               0x01, i2c_read},
	{MAX9286_ADDR,      {0x29, 0x00}, 0x00,               0x01, i2c_read},
	{MAX9286_ADDR,      {0x2A, 0x00}, 0x00,               0x01, i2c_read},
	{MAX9286_ADDR,      {0x2A, 0x00}, 0x00,               0x01, i2c_read},
	{MAX9286_ADDR,      {0x2B, 0x00}, 0x00,               0x01, i2c_read},
	{MAX9286_ADDR,      {0x2B, 0x00}, 0x00,               0x01, i2c_read},

	{MAX9271_INIT_ADDR, {0x08, 0x00}, 0x01,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x3B, 0x00}, 0x19,               0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x04, 0x00}, 0x43,               0x01, i2c_write},

	{MAX9271_INIT_ADDR, {0x06, 0x00}, 0xA0,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x15, 0x00}, 0x13,               0x01, i2c_write},

	{MAX9286_ADDR,      {0x12, 0x00}, 0xF3,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x01, 0x00}, 0x02,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x02, 0x00}, 0x20,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x63, 0x00}, 0x00,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x64, 0x00}, 0x00,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x05, 0x00}, 0x19,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x00, 0x00}, 0xEF,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x0A, 0x00}, 0xFF,               0x01, i2c_write},

	{MAX9271_INIT_ADDR, {0x07, 0x00}, 0x84,               0x01, i2c_write},

	{MAX9286_ADDR,	    {0x0A, 0x00}, 0xF1,	              0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x00, 0x00}, 0x00,               0x01, i2c_read},

	{MAX9271_INIT_ADDR, {0x00, 0x00}, MAX9271_CH0_ADDR,   0x01, i2c_write},
	{MAX9271_CH0_ADDR,  {0x07, 0x00}, 0x84,	              0x01, i2c_write},
	{MAX9271_CH0_ADDR,  {0x09, 0x00}, OV490_CH0_MAP_ADDR, 0x01, i2c_write},
	{MAX9271_CH0_ADDR,  {0x0A, 0x00}, OV490_INIT_ADDR,    0x01, i2c_write},
	{MAX9271_CH0_ADDR,  {0x0B, 0x00}, 0x8A,	              0x01, i2c_write},
	{MAX9271_CH0_ADDR,  {0x0C, 0x00}, MAX9271_CH0_ADDR,   0x01, i2c_write},

	{MAX9286_ADDR,	    {0x0A, 0x00}, 0xF2,	              0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x00, 0x00}, MAX9271_CH1_ADDR,   0x01, i2c_write},
	{MAX9271_CH1_ADDR,  {0x07, 0x00}, 0x84,	              0x01, i2c_write},
	{MAX9271_CH1_ADDR,  {0x09, 0x00}, OV490_CH1_MAP_ADDR, 0x01, i2c_write},
	{MAX9271_CH1_ADDR,  {0x0A, 0x00}, OV490_INIT_ADDR,    0x01, i2c_write},
	{MAX9271_CH1_ADDR,  {0x0B, 0x00}, 0x8A,		      0x01, i2c_write},
	{MAX9271_CH1_ADDR,  {0x0C, 0x00}, MAX9271_CH1_ADDR,   0x01, i2c_write},

	{MAX9286_ADDR,	    {0x0A, 0x00}, 0xF4,	              0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x00, 0x00}, MAX9271_CH2_ADDR,   0x01, i2c_write},
	{MAX9271_CH2_ADDR,  {0x07, 0x00}, 0x84,	              0x01, i2c_write},
	{MAX9271_CH2_ADDR,  {0x09, 0x00}, OV490_CH2_MAP_ADDR, 0x01, i2c_write},
	{MAX9271_CH2_ADDR,  {0x0A, 0x00}, OV490_INIT_ADDR,    0x01, i2c_write},
	{MAX9271_CH2_ADDR,  {0x0B, 0x00}, 0x8A,	              0x01, i2c_write},
	{MAX9271_CH2_ADDR,  {0x0C, 0x00}, MAX9271_CH2_ADDR,   0x01, i2c_write},

	{MAX9286_ADDR,	    {0x0A, 0x00}, 0xF8,	              0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x00, 0x00}, MAX9271_CH3_ADDR,   0x01, i2c_write},
	{MAX9271_CH3_ADDR,  {0x07, 0x00}, 0x84,	              0x01, i2c_write},
	{MAX9271_CH3_ADDR,  {0x09, 0x00}, OV490_CH3_MAP_ADDR, 0x01, i2c_write},
	{MAX9271_CH3_ADDR,  {0x0A, 0x00}, OV490_INIT_ADDR,    0x01, i2c_write},
	{MAX9271_CH3_ADDR,  {0x0B, 0x00}, 0x8A,		      0x01, i2c_write},
	{MAX9271_CH3_ADDR,  {0x0C, 0x00}, MAX9271_CH3_ADDR,   0x01, i2c_write},

	{MAX9286_ADDR,	    {0x0A, 0x00}, 0xFF,               0x01, i2c_write},

	{MAX9271_ALL_ADDR,  {0x04, 0x00}, 0x83,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x15, 0x00}, 0x9B,               0x01, i2c_write},

	{OV490_INIT_ADDR,   {0xFF, 0xFD}, 0x80,               0x03, i2c_write},
	{OV490_INIT_ADDR,   {0xFF, 0xFE}, 0x19,               0x03, i2c_write},
	{OV490_INIT_ADDR,   {0x50, 0x00}, 0x00,               0x03, i2c_write},
	{OV490_INIT_ADDR,   {0xFF, 0xFE}, 0x80,               0x03, i2c_write},
	{OV490_INIT_ADDR,   {0x00, 0xC0}, 0xD7,               0x03, i2c_write},

	{OV490_INIT_ADDR,   {0xFF, 0xFD}, 0x80,               0x03, i2c_write},
	{OV490_INIT_ADDR,   {0xFF, 0xFE}, 0x19,               0x03, i2c_write},
	{OV490_INIT_ADDR,   {0x50, 0x00}, 0x01,               0x03, i2c_write},
	{OV490_INIT_ADDR,   {0xFF, 0xFE}, 0x80,               0x03, i2c_write},
	{OV490_INIT_ADDR,   {0x00, 0xC0}, 0xD6,               0x03, i2c_write},
};

static struct reg_val_ops MAX9286_CAB888_4V4_r_cmd[] = {
	{MAX9271_CH0_ADDR, {0x00, 0x00}, MAX9271_CH0_ADDR,    0x01, i2c_read},
	{MAX9271_CH1_ADDR, {0x00, 0x00}, MAX9271_CH1_ADDR,    0x01, i2c_read},
	{MAX9271_CH2_ADDR, {0x00, 0x00}, MAX9271_CH2_ADDR,    0x01, i2c_read},
	{MAX9271_CH3_ADDR, {0x00, 0x00}, MAX9271_CH3_ADDR,    0x01, i2c_read},
	{MAX9286_ADDR,     {0x15, 0x00}, 0x9B,                0x01, i2c_read},
};

static struct reg_val_ops MAX9286_CAB888_4V4_pre_init_cmd[] = {
	{MAX9286_ADDR,      {0x3F, 0x00}, 0x4F,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x00, 0x00}, 0x02,               0x01, i2c_delay},
	{MAX9286_ADDR,      {0x3B, 0x00}, 0x1E,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x00, 0x00}, 0x02,               0x01, i2c_delay},
	{MAX9271_INIT_ADDR, {0x04, 0x00}, 0x43,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x00, 0x00}, 0x05,               0x01, i2c_delay},
	{MAX9286_ADDR,      {0x28, 0x00}, 0x00,               0x01, i2c_read},
	{MAX9286_ADDR,      {0x28, 0x00}, 0x00,               0x01, i2c_read},
	{MAX9286_ADDR,      {0x29, 0x00}, 0x00,               0x01, i2c_read},
	{MAX9286_ADDR,      {0x29, 0x00}, 0x00,               0x01, i2c_read},
	{MAX9286_ADDR,      {0x2A, 0x00}, 0x00,               0x01, i2c_read},
	{MAX9286_ADDR,      {0x2A, 0x00}, 0x00,               0x01, i2c_read},
	{MAX9286_ADDR,      {0x2B, 0x00}, 0x00,               0x01, i2c_read},
	{MAX9286_ADDR,      {0x2B, 0x00}, 0x00,               0x01, i2c_read},

	{MAX9271_INIT_ADDR, {0x08, 0x00}, 0x01,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x00, 0x00}, 0x02,               0x01, i2c_delay},
	{MAX9286_ADDR,      {0x3B, 0x00}, 0x19,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x00, 0x00}, 0x02,               0x01, i2c_delay},
	{MAX9286_ADDR,	    {0x34, 0x00}, 0x00,               0x01, i2c_read},

	{MAX9286_ADDR,      {0x15, 0x00}, 0x13,               0x01, i2c_write},

	{MAX9286_ADDR,      {0x12, 0x00}, 0xF3,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x01, 0x00}, 0x02,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x02, 0x00}, 0x20,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x63, 0x00}, 0x00,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x64, 0x00}, 0x00,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x05, 0x00}, 0x19,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x00, 0x00}, 0xEF,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x0A, 0x00}, 0xFF,               0x01, i2c_write},

	{MAX9271_INIT_ADDR, {0x07, 0x00}, 0x84,               0x01, i2c_write},
};

static struct reg_val_ops MAX9286_CAB888_CH0_addr_init_cmd[] = {
	{MAX9286_ADDR,	    {0x0A, 0x00}, 0xF1,	              0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x00, 0x00}, 0x80,               0x01, i2c_read},
	{MAX9271_CH0_ADDR,  {0x00, 0x00}, MAX9271_CH0_ADDR,   0x01, i2c_read},
	{MAX9271_CH0_ADDR, {0x00, 0x00},  MAX9271_INIT_ADDR,  0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x00, 0x00}, MAX9271_CH0_ADDR,   0x01, i2c_read},
	{MAX9271_INIT_ADDR, {0x00, 0x00}, MAX9271_CH0_ADDR,   0x01, i2c_write},
	{MAX9271_CH0_ADDR,  {0x07, 0x00}, 0x84,	              0x01, i2c_write},
	{MAX9271_CH0_ADDR,  {0x09, 0x00}, OV490_CH0_MAP_ADDR, 0x01, i2c_write},
	{MAX9271_CH0_ADDR,  {0x0A, 0x00}, OV490_INIT_ADDR,    0x01, i2c_write},
	{MAX9271_CH0_ADDR,  {0x0B, 0x00}, MAX9271_ALL_ADDR,   0x01, i2c_write},
	{MAX9271_CH0_ADDR,  {0x0C, 0x00}, MAX9271_CH0_ADDR,   0x01, i2c_write},
};

static struct reg_val_ops MAX9286_CAB888_CH1_addr_init_cmd[] = {
	{MAX9286_ADDR,	    {0x0A, 0x00}, 0xF2,               0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x00, 0x00}, 0x80,               0x01, i2c_read},
	{MAX9271_CH1_ADDR,  {0x00, 0x00}, MAX9271_CH1_ADDR,   0x01, i2c_read},
	{MAX9271_CH1_ADDR,  {0x00, 0x00}, MAX9271_INIT_ADDR,  0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x00, 0x00}, MAX9271_CH1_ADDR,   0x01, i2c_read},
	{MAX9271_INIT_ADDR, {0x00, 0x00}, MAX9271_CH1_ADDR,   0x01, i2c_write},
	{MAX9271_CH1_ADDR,  {0x07, 0x00}, 0x84,               0x01, i2c_write},
	{MAX9271_CH1_ADDR,  {0x09, 0x00}, OV490_CH1_MAP_ADDR, 0x01, i2c_write},
	{MAX9271_CH1_ADDR,  {0x0A, 0x00}, OV490_INIT_ADDR,    0x01, i2c_write},
	{MAX9271_CH1_ADDR,  {0x0B, 0x00}, MAX9271_ALL_ADDR,   0x01, i2c_write},
	{MAX9271_CH1_ADDR,  {0x0C, 0x00}, MAX9271_CH1_ADDR,   0x01, i2c_write},
};

static struct reg_val_ops MAX9286_CAB888_CH2_addr_init_cmd[] = {
	{MAX9286_ADDR,	    {0x0A, 0x00}, 0xF4,	              0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x00, 0x00}, 0x80,               0x01, i2c_read},
	{MAX9271_CH2_ADDR,  {0x00, 0x00}, MAX9271_CH2_ADDR,   0x01, i2c_read},
	{MAX9271_CH2_ADDR,  {0x00, 0x00}, MAX9271_INIT_ADDR,  0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x00, 0x00}, MAX9271_CH2_ADDR,   0x01, i2c_read},
	{MAX9271_INIT_ADDR, {0x00, 0x00}, MAX9271_CH2_ADDR,   0x01, i2c_write},
	{MAX9271_CH2_ADDR,  {0x07, 0x00}, 0x84,	              0x01, i2c_write},
	{MAX9271_CH2_ADDR,  {0x09, 0x00}, OV490_CH2_MAP_ADDR, 0x01, i2c_write},
	{MAX9271_CH2_ADDR,  {0x0A, 0x00}, OV490_INIT_ADDR,    0x01, i2c_write},
	{MAX9271_CH2_ADDR,  {0x0B, 0x00}, MAX9271_ALL_ADDR,   0x01, i2c_write},
	{MAX9271_CH2_ADDR,  {0x0C, 0x00}, MAX9271_CH2_ADDR,   0x01, i2c_write},
};

static struct reg_val_ops MAX9286_CAB888_CH3_addr_init_cmd[] = {
	{MAX9286_ADDR,	    {0x0A, 0x00}, 0xF8,	              0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x00, 0x00}, 0x80,               0x01, i2c_read},
	{MAX9271_CH3_ADDR,  {0x00, 0x00}, MAX9271_CH3_ADDR,   0x01, i2c_read},
	{MAX9271_CH3_ADDR,  {0x00, 0x00}, MAX9271_INIT_ADDR,  0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x00, 0x00}, MAX9271_CH3_ADDR,   0x01, i2c_read},
	{MAX9271_INIT_ADDR, {0x00, 0x00}, MAX9271_CH3_ADDR,   0x01, i2c_write},
	{MAX9271_CH3_ADDR,  {0x07, 0x00}, 0x84,	              0x01, i2c_write},
	{MAX9271_CH3_ADDR,  {0x09, 0x00}, OV490_CH3_MAP_ADDR, 0x01, i2c_write},
	{MAX9271_CH3_ADDR,  {0x0A, 0x00}, OV490_INIT_ADDR,    0x01, i2c_write},
	{MAX9271_CH3_ADDR,  {0x0B, 0x00}, MAX9271_ALL_ADDR,   0x01, i2c_write},
	{MAX9271_CH3_ADDR,  {0x0C, 0x00}, MAX9271_CH3_ADDR,   0x01, i2c_write},
};

static struct reg_val_ops MAX9286_CAB888_4V4_en_cmd[] = {
	{MAX9286_ADDR,	    {0x0A, 0x00}, 0xFF,               0x01, i2c_write},
	{MAX9271_ALL_ADDR,  {0x04, 0x00}, 0x83,               0x01, i2c_write},
	{MAX9286_ADDR,      {0x15, 0x00}, 0x9B,               0x01, i2c_write},
};

static struct reg_val_ops MAX9286_CAB888_1v1_init_cmd[] = {
	{MAX9286_ADDR,      {0x0D, 0x00}, 0x03,             0x01, i2c_write},
	{MAX9286_ADDR,      {0x3F, 0x00}, 0x4F,             0x01, i2c_write},
	{MAX9286_ADDR,      {0x3B, 0x00}, 0x1E,             0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x04, 0x00}, 0x07,             0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x06, 0x00}, 0xA0,             0x01, i2c_write},
	{MAX9286_ADDR,      {0x15, 0x00}, 0x03,             0x01, i2c_write},
#if MAX9286_1CH_LANE == 1
	{MAX9286_ADDR,      {0x12, 0x00}, 0x33,             0x01, i2c_write},
#elif MAX9286_1CH_LANE == 4
	{MAX9286_ADDR,      {0x12, 0x00}, 0xF3,             0x01, i2c_write},
#endif
	{MAX9286_ADDR,      {0x01, 0x00}, 0xE2,             0x01, i2c_write},
	{MAX9286_ADDR,      {0x00, 0x00}, 0x81,             0x01, i2c_write},

	{MAX9286_ADDR,      {0x0A, 0x00}, 0xF1,             0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x07, 0x00}, 0x84,             0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x09, 0x00}, OV490_CH0_ADDR,   0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x0A, 0x00}, OV490_INIT_ADDR,  0x01, i2c_write},
	{MAX9286_ADDR,      {0x34, 0x00}, 0xB6,             0x01, i2c_write},
	{MAX9271_INIT_ADDR, {0x04, 0x00}, 0xC7,             0x01, i2c_write},
	{MAX9286_ADDR,      {0x15, 0x00}, 0x0B,             0x01, i2c_write},

	{OV490_CH0_ADDR,    {0xFF, 0xFD}, 0x80,             0x02, i2c_write},
	{OV490_CH0_ADDR,    {0xFF, 0xFE}, 0x19,             0x02, i2c_write},
	{OV490_CH0_ADDR,    {0x50, 0x00}, 0x03,             0x02, i2c_write},
	{OV490_CH0_ADDR,    {0xFF, 0xFE}, 0x80,             0x02, i2c_write},
	{OV490_CH0_ADDR,    {0x00, 0xC0}, 0xD6,             0x02, i2c_write},
};

struct max9286_datafmt {
	__u32 code;
	enum v4l2_colorspace colorspace;
};

struct max9286 {
	struct v4l2_subdev		subdev;
	const struct max9286_datafmt	*fmt;
	struct v4l2_clk			*clk;
	struct i2c_client *client;

	/* blanking information */
	u32 link;
};

static const struct max9286_datafmt max9286_colour_fmts[] = {
	{MEDIA_BUS_FMT_YUYV8_2X8, V4L2_COLORSPACE_DEFAULT},
};

static struct max9286 *to_max9286(struct i2c_client *client)
{
	return container_of(i2c_get_clientdata(client), struct max9286, subdev);
}

static int i2c_delay(struct i2c_client *client, u16 slave_addr,
		u8 *reg, unsigned int reg_len, u8 *value)
{
	max9286_info("delay %d ms", *value);
	mdelay(*value);

	return 0;
}

static int i2c_read(struct i2c_client *client, u16 slave_addr,
		u8 *reg, unsigned int reg_len, u8 *val)
{
	int ret = 0;
	u8 *data = NULL;
	struct i2c_msg msg[2];
	unsigned int size = reg_len + 1u;

	if ((reg == NULL) || (val == NULL) || (reg_len <= 0u)) {
		max9286_err("reg/val/reg_len is %02x/%02x/%d",
			*reg, *val, reg_len);
		return -EINVAL;
	}

	data = kzalloc(size, GFP_KERNEL);
	if (data == NULL)
		return -ENOSPC;

	(void)memcpy(data, reg, reg_len);
	(void)memset(msg, 0, sizeof(msg));

	msg[0].addr = (slave_addr >> 1);
	msg[0].flags = 0;
	msg[0].len = (__u16)reg_len;
	msg[0].buf = data;

	msg[1].addr = (slave_addr >> 1);
	msg[1].flags = I2C_M_RD;
	msg[1].len = 1;
	msg[1].buf = data + reg_len;

	client->addr = (slave_addr >> 1);

	ret = i2c_transfer(client->adapter, msg, 2);

	*val = *(data + reg_len);
	kfree(data);
	data = NULL;
	max9286_info("read dev/reg/val/ret is %02x/%02x/%02x/%d",
		slave_addr, *reg, *val, ret);

	return ret;
}

static int i2c_write(struct i2c_client *client, u16 slave_addr,
		u8 *reg, unsigned int reg_len, u8 *val)
{
	u8 *data = NULL;
	int ret = 0;
	struct i2c_msg msg;
	unsigned int size = reg_len + 1u;

	if ((reg == NULL) || (val == NULL) || (reg_len <= 0u)) {
		max9286_err("reg/val/reg_len is %02x/%02x/%d",
			*reg, *val, reg_len);
		return -EINVAL;
	}

	data = kzalloc(size, GFP_KERNEL);
	if (data == NULL)
		return -ENOSPC;

	(void)memcpy(data, reg, reg_len);
	*(data + reg_len) = *val;

	(void)memset(&msg, 0, sizeof(msg));
	msg.addr = (slave_addr >> 1);
	msg.flags = 0;
	msg.len = (u16)size;
	msg.buf = data;

	client->addr = (slave_addr >> 1);
	ret = i2c_transfer(client->adapter, &msg, 1);

	kfree(data);
	data = NULL;
	max9286_info("write dev/reg/val/ret is %02x/%02x/%02x/%d",
		slave_addr, *reg, *val, ret);
	return ret;
}

static int read_max9286_id(struct i2c_client *client, u8 *id_val)
{
	int ret = 0;
	u8 max9286_id_reg = MAX9286_ID_REG;

	ret = i2c_read(client, MAX9286_ADDR, &max9286_id_reg, 1, id_val);
	if (ret != 2) {
		max9286_err("ret=%d", ret);
		return -EIO;
	}

	if (*id_val != (u8)MAX9286_ID) {
		max9286_err("max9286 ID not match. Default is %x but read from register is %x. ret=%d",
			MAX9286_ID, *id_val, ret);
		ret = -ENODEV;
	} else {
		max9286_info("max9286 ID match. Default is %x and read from register is %x. ret=%d",
			MAX9286_ID, *id_val, ret);
	}

	return ret;
}

static int max9286_write_array(struct i2c_client *client,
		struct reg_val_ops *cmd, unsigned long len)
{
	int ret = 0;
	unsigned long index = 0;

	for (; index < len; ++index) {
		ret = cmd[index].i2c_ops(client, cmd[index].slave_addr,
			cmd[index].reg, cmd[index].reg_len, &(cmd[index].val));
		if (ret < 0) {
			max9286_err("dev/reg/val/ret/index %x/%x/%x/%d/%lu",
				cmd[index].slave_addr, cmd[index].reg[0],
				cmd[index].val, ret, index);
			return ret;
		}
	}

	return 0;
}

static int max9286_set_link_config(struct i2c_client *client, u8 *val)
{
	int ret = 0;
	u8 link_config_reg = MAX9286_LINK_CONFIG_REG;

	ret = i2c_write(client, MAX9286_ADDR, &link_config_reg, 1, val);
	if (ret != 1) {
		max9286_info("link config set fail. ret = %d", ret);
		return -EIO;
	}

	return ret;
}

static int max9286_get_lock_status(struct i2c_client *client, u8 *val)
{
	int ret = 0;
	u8 lock_reg = MAX9286_LOCK_REG;

	ret = i2c_read(client, MAX9286_ADDR, &lock_reg, 1, val);
	if (ret != 2) {
		max9286_err("ret=%d", ret);
		return -EIO;
	}

	if ((*val & (u8)0x80) != 0u) {
		max9286_info("camera links are locked");
	} else {
		max9286_err("camera links are not locked");
		ret =  -ENODEV;
	}

	return ret;
}

static int max9286_get_link(struct i2c_client *client, u8 *val)
{
	int ret = 0;
	u8 link_reg = MAX9286_LINK_REG;

	ret = i2c_read(client, MAX9286_ADDR, &link_reg, 1, val);
	if (ret != 2) {
		max9286_err("ret=%d", ret);
		ret = -EIO;
	}

	return ret;
}

static int max9286_camera_ch_addr_init(struct i2c_client *client,
	struct reg_val_ops *cmd, unsigned long len, int ch)
{
	int ret = 0;
	unsigned long index = 0;

	/*choose channel*/
	ret = cmd[index].i2c_ops(client, cmd[index].slave_addr,
		cmd[index].reg, cmd[index].reg_len, &(cmd[index].val));
	if (ret < 0) {
		max9286_err("dev/reg/val/ret/index %x/%x/%x/%d/%lu",
			cmd[index].slave_addr, cmd[index].reg[0],
			cmd[index].val, ret, index);
		return -1;
	}

	index = 1;
	/*read max9271 id use 0x80*/
	ret = cmd[index].i2c_ops(client, cmd[index].slave_addr,
		cmd[index].reg, cmd[index].reg_len, &(cmd[index].val));
	/*
	 * device is not exist. Maybe device address has been changed but
	 * device is not reset because power off time is too short.
	 * Now reset it.
	 */
	if (ret < 0) {
		/*use channel device id to read*/
		index = 2;
		ret = cmd[index].i2c_ops(client, cmd[index].slave_addr,
			cmd[index].reg, cmd[index].reg_len, &(cmd[index].val));
		if (ret < 0) {
			max9286_err("dev/reg/val/ret/index %x/%x/%x/%d/%lu",
				cmd[index].slave_addr, cmd[index].reg[0],
				cmd[index].val, ret, index);
			return -1;
		}
		max9286_info("ch %d addr %02x has not been cleared",
			ch, cmd[index].val);

		/*use channel id to set address to 0x80*/
		index = 3;
		ret =  cmd[index].i2c_ops(client, cmd[index].slave_addr,
			cmd[index].reg, cmd[index].reg_len, &(cmd[index].val));
		if (ret < 0) {
			max9286_err("dev/reg/val/ret/index %x/%x/%x/%d/%lu",
				cmd[index].slave_addr, cmd[index].reg[0],
				cmd[index].val, ret, index);
			return -1;
		}

		/*use 0x80 to read id*/
		index = 4;
		ret =  cmd[index].i2c_ops(client, cmd[index].slave_addr,
			cmd[index].reg, cmd[index].reg_len, &(cmd[index].val));
		if (ret < 0) {
			max9286_err("dev/reg/val/ret/index %x/%x/%x/%d/%lu",
				cmd[index].slave_addr, cmd[index].reg[0],
				cmd[index].val, ret, index);
			return -1;
		}

	}

	for (index = 5; index < len; ++index) {
		ret =  cmd[index].i2c_ops(client, cmd[index].slave_addr,
			cmd[index].reg, cmd[index].reg_len, &(cmd[index].val));
		if (ret < 0) {
			max9286_err("dev/reg/val/ret/index %x/%x/%x/%d/%lu",
				cmd[index].slave_addr, cmd[index].reg[0],
				cmd[index].val, ret, index);
			return -1;
		}
	}

	return 0;
}

static int max9286_cab888_4v4_is_init(struct i2c_client *client)
{
	unsigned long len = ARRAY_SIZE(MAX9286_CAB888_4V4_r_cmd);
	struct reg_val_ops *cmd = MAX9286_CAB888_4V4_r_cmd;
	unsigned long index = 0UL;
	int ret = 0;
	u8 val = 0;

	for (index = 0; index < len; ++index) {
		val = cmd[index].val;
		ret = cmd[index].i2c_ops(client, cmd[index].slave_addr,
			cmd[index].reg, cmd[index].reg_len, &(cmd[index].val));
		if ((ret < 0) || (val != cmd[index].val))
			return -1;
	}

	return 0;
}

static int max9286_cab888_4v4_init(struct i2c_client *client)
{
	int ret = 0;

	ret  = max9286_cab888_4v4_is_init(client);
	if (ret == 0) {
		max9286_info("max9286 and cab888 have been initialized");
		return 0;
	}

	ret = max9286_write_array(client,
		MAX9286_CAB888_4V4_pre_init_cmd,
		ARRAY_SIZE(MAX9286_CAB888_4V4_pre_init_cmd));
	if (ret < 0)
		return ret;

	ret = max9286_camera_ch_addr_init(client,
		MAX9286_CAB888_CH0_addr_init_cmd,
		ARRAY_SIZE(MAX9286_CAB888_CH0_addr_init_cmd),
		0);
	if (ret < 0)
		return ret;

	ret = max9286_camera_ch_addr_init(client,
		MAX9286_CAB888_CH1_addr_init_cmd,
		ARRAY_SIZE(MAX9286_CAB888_CH1_addr_init_cmd),
		0);
	if (ret < 0)
		return ret;

	ret = max9286_camera_ch_addr_init(client,
		MAX9286_CAB888_CH2_addr_init_cmd,
		ARRAY_SIZE(MAX9286_CAB888_CH2_addr_init_cmd),
		0);
	if (ret < 0)
		return ret;

	ret = max9286_camera_ch_addr_init(client,
		MAX9286_CAB888_CH3_addr_init_cmd,
		ARRAY_SIZE(MAX9286_CAB888_CH3_addr_init_cmd),
		0);
	if (ret < 0)
		return ret;

	ret = max9286_write_array(client,
		MAX9286_CAB888_4V4_en_cmd,
		ARRAY_SIZE(MAX9286_CAB888_4V4_en_cmd));
	if (ret < 0)
		return ret;

	return 0;
}

static int max9286_camera_init(struct i2c_client *client)
{
	int ret = 0;
	u8 max9286_id_val = 0;
	u8 lock_reg_val = 0;
	u8 link_config_reg_val = 0x81;
	u8 link_reg_val = 0;
	u8 ch_link_status[4] = {0};
	int index = 0;
	int read_cnt = 0;
	u32 link_cnt = 0;
	struct max9286 *priv = NULL;
	unsigned long skip_len = 0;
	u64 delay = 10000;

	/*MAX9286 ID confirm*/
	for (read_cnt = 0; read_cnt < 3; ++read_cnt) {
		ret = read_max9286_id(client, &max9286_id_val);
		if (ret == 2)
			break;

		if (read_cnt == 2) {
			max9286_err("read max9286 ID time out");
			return -EIO;
		}

		udelay(delay);
	}

	/*check video link*/
	for (read_cnt = 0; read_cnt < 3; ++read_cnt) {
		ret = max9286_get_link(client, &link_reg_val);
		if ((ret == 2) && (link_reg_val > 0u))
			break;

		if (read_cnt == 2) {
			max9286_err("no camera linked link_reg_val=%x",
				link_reg_val);
			return -ENODEV;
		}

		udelay(delay);
	}

	for (index = 0; index < 4; ++index) {
		ch_link_status[index] = link_reg_val & (u8)0x1;
		link_reg_val >>= 1;

		if (ch_link_status[index] != 0u) {
			++link_cnt;
			max9286_info("channel %d linked", index);
		} else {
			max9286_info("channel %d not linked", index);
		}
	}

	/* If link_cnt < 4. Link config should be set or lock status error */
	if (link_cnt == 1U) {
		ret = max9286_set_link_config(client, &link_config_reg_val);
		if (ret != 1)
			return -EIO;

	}

	max9286_info("%u camera linked", link_cnt);

	/* check camera links are locked or not */
	for (read_cnt = 0; read_cnt < 3; ++read_cnt) {
		ret = max9286_get_lock_status(client, &lock_reg_val);
		if (ret == 2)
			break;

		if (read_cnt == 2) {
			max9286_err("read lock status time out");
			return -EIO;
		}

		udelay(delay);
	}

	priv = to_max9286(client);
	priv->link = link_cnt;

	/*init max9286 command*/
	if (link_cnt == 1U) {
		skip_len = 5;
		ret = max9286_write_array(client,
			MAX9286_CAB888_1v1_init_cmd,
			ARRAY_SIZE(MAX9286_CAB888_1v1_init_cmd) - skip_len);
	} else if (link_cnt == 4U) {
		ret = max9286_cab888_4v4_init(client);
		if (ret < 0)
			return ret;
	} else {
		max9286_err("error link cnt %u", link_cnt);
		ret = -EINVAL;
	}

	return ret;
}

static int max9286_g_mbus_config(struct v4l2_subdev *sd,
				struct v4l2_mbus_config *cfg)
{
	cfg->type = V4L2_MBUS_CSI2;
	cfg->flags = V4L2_MBUS_CSI2_4_LANE | V4L2_MBUS_CSI2_CHANNELS |
		     V4L2_MBUS_CSI2_CONTINUOUS_CLOCK;
	return 0;
}

static int max9286_s_stream(struct v4l2_subdev *sd, int enable)
{
	return 0;
}

static int max9286_s_mbus_config(struct v4l2_subdev *sd,
			     const struct v4l2_mbus_config *cfg)
{
	return 0;
}

static int max9286_s_power(struct v4l2_subdev *sd, int on)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct soc_camera_subdev_desc *ssdd = soc_camera_i2c_to_desc(client);
	struct max9286 *priv = to_max9286(client);

	if (on == 0)
		return soc_camera_power_off(&client->dev, ssdd, priv->clk);

	return soc_camera_power_on(&client->dev, ssdd, priv->clk);
}

static int max9286_enum_mbus_code(struct v4l2_subdev *sd,
		struct v4l2_subdev_pad_config *cfg,
		struct v4l2_subdev_mbus_code_enum *code)
{
	if ((code->pad != 0u) ||
	    (unsigned long)code->index >= ARRAY_SIZE(max9286_colour_fmts))
		return -EINVAL;

	code->code = max9286_colour_fmts[code->index].code;
	return 0;
}

static const struct max9286_datafmt *max9286_find_datafmt(u32 code)
{
	__u64 i;

	for (i = 0; i < ARRAY_SIZE(max9286_colour_fmts); i++)
		if (max9286_colour_fmts[i].code == code)
			return max9286_colour_fmts + i;

	return NULL;
}

static int max9286_set_fmt(struct v4l2_subdev *sd,
		struct v4l2_subdev_pad_config *cfg,
		struct v4l2_subdev_format *format)
{
	struct v4l2_mbus_framefmt *mf = &format->format;
	const struct max9286_datafmt *fmt = max9286_find_datafmt(mf->code);
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct max9286 *priv = to_max9286(client);
	int ret = 0;
	unsigned long cmd_len = 0;
	unsigned long skip = 0;

	if (format->pad != 0u)
		return -EINVAL;

	if (fmt == NULL) {
		/* MIPI CSI could have changed the format, double-check */
		if (format->which == (__u32)V4L2_SUBDEV_FORMAT_ACTIVE)
			return -EINVAL;
		mf->code	= max9286_colour_fmts[0].code;
		mf->colorspace	= (__u32)max9286_colour_fmts[0].colorspace;
	}

	if ((mf->width != (__u32)CAB888_WIDTH) ||
		(mf->height != (__u32)CAB888_HEIGHT * priv->link)) {
		max9286_err("width/height %d/%d not support. Set %d/%d as default",
			mf->width, mf->height,
			CAB888_WIDTH, CAB888_HEIGHT * priv->link);
	}

	mf->width	= CAB888_WIDTH;
	mf->height	= CAB888_HEIGHT * priv->link;
	mf->field	= (__u32)V4L2_FIELD_NONE;

	if (format->which == (__u32)V4L2_SUBDEV_FORMAT_ACTIVE)
		priv->fmt = max9286_find_datafmt(mf->code);
	else
		cfg->try_fmt = *mf;

	if (is_testpattern > 0) {
		/*init max9286 command*/
		if (priv->link == 1U) {
			cmd_len = ARRAY_SIZE(MAX9286_CAB888_1v1_init_cmd);
			skip = 5;
			ret = max9286_write_array(client,
				MAX9286_CAB888_1v1_init_cmd + cmd_len - skip,
				5);
		} else if (priv->link == 4U) {
			cmd_len = ARRAY_SIZE(MAX9286_CAB888_4v4_init_cmd);
			skip = 10;
			ret = max9286_write_array(client,
				MAX9286_CAB888_4v4_init_cmd + cmd_len - skip,
				10);
		} else {
			max9286_err("error link cnt %d", priv->link);
			ret = -EINVAL;
		}
	}
	return ret;
}

static int max9286_g_register(struct v4l2_subdev *sd,
		struct v4l2_dbg_register *reg)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret = 0;
	u8 val = 0;
	u8 max9286_reg = (u8)(reg->reg);

	if (reg == NULL)
		return -EINVAL;

	max9286_info("read max9286 reg %llx", reg->reg);
	if (reg->match.type == (u8)0) {
		ret = i2c_read(client, MAX9286_ADDR, &max9286_reg, 1, &val);
		if (ret != 2) {
			max9286_err("ret=%d", ret);
			return -EIO;
		}
		reg->val = val;
	}

	return ret;
}

static const struct v4l2_subdev_video_ops max9286_subdev_video_ops = {
	.g_mbus_config	= max9286_g_mbus_config,
	.s_stream	= max9286_s_stream,
	.s_mbus_config = max9286_s_mbus_config,
};

static const struct v4l2_subdev_core_ops max9286_subdev_core_ops = {
	.s_power	= max9286_s_power,
	.g_register	= max9286_g_register,
};

static const struct v4l2_subdev_pad_ops max9286_subdev_pad_ops = {
	.enum_mbus_code = max9286_enum_mbus_code,
	.set_fmt	= max9286_set_fmt,
};

static const struct v4l2_subdev_ops max9286_subdev_ops = {
	.core	= &max9286_subdev_core_ops,
	.video	= &max9286_subdev_video_ops,
	.pad = &max9286_subdev_pad_ops,
};

static int max9286_probe(struct i2c_client *client,
			const struct i2c_device_id *did)
{
	struct max9286 *priv = NULL;
	struct soc_camera_subdev_desc *ssdd = soc_camera_i2c_to_desc(client);
	struct v4l2_subdev *subdev = NULL;
	int ret = 0;

	if (client->dev.of_node != NULL) {
		ssdd = devm_kzalloc(&client->dev, sizeof(*ssdd), GFP_KERNEL);
		if (ssdd == NULL)
			return -ENOMEM;
		client->dev.platform_data = ssdd;
	}

	if (ssdd == NULL) {
		max9286_err("MAX9286: missing platform data");
		return -EINVAL;
	}

	priv = devm_kzalloc(&client->dev, sizeof(struct max9286), GFP_KERNEL);
	if (priv == NULL)
		return -ENOMEM;

	v4l2_i2c_subdev_init(&priv->subdev, client, &max9286_subdev_ops);

	priv->fmt		= &max9286_colour_fmts[0];

	subdev = i2c_get_clientdata(client);

	ret =  max9286_camera_init(client);
	if (ret < 0)
		return ret;

	priv->subdev.dev = &client->dev;

	return v4l2_async_register_subdev(&priv->subdev);
}

static int max9286_remove(struct i2c_client *client)
{
	struct soc_camera_subdev_desc *ssdd = soc_camera_i2c_to_desc(client);

	if (ssdd->free_bus != NULL)
		ssdd->free_bus(ssdd);

	return 0;
}

static const struct i2c_device_id max9286_id[] = {
	{ "max9286", 0},
	{ }
};
static const struct of_device_id max9286_camera_of_match[] = {
	{ .compatible = "maxim,max9286", },
	{},
};

static struct i2c_driver max9286_i2c_driver = {
	.driver = {
		.name = "max9286",
		.of_match_table = max9286_camera_of_match,
		.probe_type = PROBE_PREFER_ASYNCHRONOUS,
	},
	.probe		= max9286_probe,
	.remove		= max9286_remove,
	.id_table	= max9286_id,
};

module_param(is_testpattern, int, 0644);
MODULE_PARM_DESC(is_testpattern, "Whether the MAX9286 get test pattern data");
module_i2c_driver(max9286_i2c_driver);

MODULE_DESCRIPTION("MAXIM Camera driver");
MODULE_AUTHOR("Baoyin Zhang<baoyin.zhang@mediatek.com");
MODULE_LICENSE("GPL v2");
