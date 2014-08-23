/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * ST-Ericsson B2R2 filters.
 *
 * Author: Fredrik Allansson <fredrik.allansson@stericsson.com> for ST-Ericsson.
 *
 * License terms: GNU General Public License (GPL), version 2.
 */

#include <linux/dma-mapping.h>

#include "b2r2_filters.h"
#include "b2r2_internal.h"
#include "b2r2_debug.h"

/**
 * struct b2r2_filter_spec filters[] - Filter lookup table
 *
 * Lookup table for filters for different scale factors. A filter
 * will be selected according to "min < scale_factor <= max".
 */
static struct b2r2_filter_spec filters[] = {
	{
		.min = 1024,
		.max = 1433,
		.h_coeffs =  {
			0x00, 0x00, 0xFA, 0x09, 0x34, 0x09, 0x00, 0x00,
			0x00, 0x00, 0xF9, 0x10, 0x32, 0x06, 0xFF, 0x00,
			0x00, 0x00, 0xF8, 0x17, 0x2F, 0x02, 0x00, 0x00,
			0x00, 0x00, 0xF7, 0x20, 0x2A, 0xFF, 0x00, 0x00,
			0x00, 0x00, 0xF8, 0x27, 0x25, 0xFC, 0x00, 0x00,
			0x00, 0x00, 0xFA, 0x2D, 0x1D, 0xFC, 0x00, 0x00,
			0x00, 0x00, 0xFE, 0x31, 0x15, 0xFC, 0x00, 0x00,
			0x00, 0x00, 0x02, 0x35, 0x0D, 0xFC, 0x00, 0x00
		},
		.v_coeffs = {
			0x00, 0x02, 0x3C, 0x02, 0x00,
			0x00, 0x08, 0x3B, 0xFD, 0x00,
			0x00, 0x10, 0x35, 0xFB, 0x00,
			0x00, 0x18, 0x30, 0xF8, 0x00,
			0x00, 0x1F, 0x27, 0xFA, 0x00,
			0x00, 0x27, 0x1E, 0xFB, 0x00,
			0x00, 0x2E, 0x16, 0xFC, 0x00,
			0x00, 0x34, 0x0D, 0xFF, 0x00
		},
	},
	{
		.min = 1433,
		.max = 1536,
		.h_coeffs =  {
			0xfe, 0x06, 0xf8, 0x0b, 0x30, 0x0b, 0xf8, 0x06,
			0xff, 0x06, 0xf7, 0x12, 0x2d, 0x05, 0xfa, 0x06,
			0x00, 0x04, 0xf6, 0x18, 0x2c, 0x00, 0xfc, 0x06,
			0x01, 0x02, 0xf7, 0x1f, 0x27, 0xfd, 0xff, 0x04,
			0x03, 0x00, 0xf9, 0x24, 0x24, 0xf9, 0x00, 0x03,
			0x04, 0xff, 0xfd, 0x29, 0x1d, 0xf7, 0x02, 0x01,
			0x06, 0xfc, 0x00, 0x2d, 0x17, 0xf6, 0x04, 0x00,
			0x06, 0xfa, 0x05, 0x30, 0x0f, 0xf7, 0x06, 0xff
		},
		.v_coeffs = {
			0xf6, 0x0e, 0x38, 0x0e, 0xf6,
			0xf5, 0x15, 0x38, 0x06, 0xf8,
			0xf5, 0x1d, 0x33, 0x00, 0xfb,
			0xf6, 0x23, 0x2d, 0xfc, 0xfe,
			0xf9, 0x28, 0x26, 0xf9, 0x00,
			0xfc, 0x2c, 0x1e, 0xf7, 0x03,
			0x00, 0x2e, 0x18, 0xf6, 0x04,
			0x05, 0x2e, 0x11, 0xf7, 0x05
		},
	},
	{
		.min = 1536,
		.max = 3072,
		.h_coeffs =  {
			0xfc, 0xfd, 0x06, 0x13, 0x18, 0x13, 0x06, 0xfd,
			0xfc, 0xfe, 0x08, 0x15, 0x17, 0x12, 0x04, 0xfc,
			0xfb, 0xfe, 0x0a, 0x16, 0x18, 0x10, 0x03, 0xfc,
			0xfb, 0x00, 0x0b, 0x18, 0x17, 0x0f, 0x01, 0xfb,
			0xfb, 0x00, 0x0d, 0x19, 0x17, 0x0d, 0x00, 0xfb,
			0xfb, 0x01, 0x0f, 0x19, 0x16, 0x0b, 0x00, 0xfb,
			0xfc, 0x03, 0x11, 0x19, 0x15, 0x09, 0xfe, 0xfb,
			0xfc, 0x04, 0x12, 0x1a, 0x12, 0x08, 0xfe, 0xfc
		},
		.v_coeffs = {
			0x05, 0x10, 0x16, 0x10, 0x05,
			0x06, 0x11, 0x16, 0x0f, 0x04,
			0x08, 0x13, 0x15, 0x0e, 0x02,
			0x09, 0x14, 0x16, 0x0c, 0x01,
			0x0b, 0x15, 0x15, 0x0b, 0x00,
			0x0d, 0x16, 0x13, 0x0a, 0x00,
			0x0f, 0x17, 0x13, 0x08, 0xff,
			0x11, 0x18, 0x12, 0x07, 0xfe
		},
	},
	{
		.min = 3072,
		.max = 4096,
		.h_coeffs =  {
			0xfe, 0x02, 0x09, 0x0f, 0x0e, 0x0f, 0x09, 0x02,
			0xff, 0x02, 0x09, 0x0f, 0x10, 0x0e, 0x08, 0x01,
			0xff, 0x03, 0x0a, 0x10, 0x10, 0x0d, 0x07, 0x00,
			0x00, 0x04, 0x0b, 0x10, 0x0f, 0x0c, 0x06, 0x00,
			0x00, 0x05, 0x0c, 0x10, 0x0e, 0x0c, 0x05, 0x00,
			0x00, 0x06, 0x0c, 0x11, 0x0e, 0x0b, 0x04, 0x00,
			0x00, 0x07, 0x0d, 0x11, 0x0f, 0x0a, 0x03, 0xff,
			0x01, 0x08, 0x0e, 0x11, 0x0e, 0x09, 0x02, 0xff
		},
		.v_coeffs = {
			0x09, 0x0f, 0x10, 0x0f, 0x09,
			0x09, 0x0f, 0x12, 0x0e, 0x08,
			0x0a, 0x10, 0x11, 0x0e, 0x07,
			0x0b, 0x11, 0x11, 0x0d, 0x06,
			0x0c, 0x11, 0x12, 0x0c, 0x05,
			0x0d, 0x12, 0x11, 0x0c, 0x04,
			0x0e, 0x12, 0x11, 0x0b, 0x04,
			0x0f, 0x13, 0x11, 0x0a, 0x03
		},
	},
	{
		.min = 4096,
		.max = 5120,
		.h_coeffs =  {
			0x00, 0x04, 0x09, 0x0c, 0x0e, 0x0c, 0x09, 0x04,
			0x01, 0x05, 0x09, 0x0c, 0x0d, 0x0c, 0x08, 0x04,
			0x01, 0x05, 0x0a, 0x0c, 0x0e, 0x0b, 0x08, 0x03,
			0x02, 0x06, 0x0a, 0x0d, 0x0c, 0x0b, 0x07, 0x03,
			0x02, 0x07, 0x0a, 0x0d, 0x0d, 0x0a, 0x07, 0x02,
			0x03, 0x07, 0x0b, 0x0d, 0x0c, 0x0a, 0x06, 0x02,
			0x03, 0x08, 0x0b, 0x0d, 0x0d, 0x0a, 0x05, 0x01,
			0x04, 0x08, 0x0c, 0x0d, 0x0c, 0x09, 0x05, 0x01
		},
		.v_coeffs = {
			0x0a, 0x0e, 0x10, 0x0e, 0x0a,
			0x0b, 0x0e, 0x0f, 0x0e, 0x0a,
			0x0b, 0x0f, 0x10, 0x0d, 0x09,
			0x0c, 0x0f, 0x10, 0x0d, 0x08,
			0x0d, 0x0f, 0x0f, 0x0d, 0x08,
			0x0d, 0x10, 0x10, 0x0c, 0x07,
			0x0e, 0x10, 0x0f, 0x0c, 0x07,
			0x0f, 0x10, 0x10, 0x0b, 0x06
		},
	},
};
static const size_t filters_size = sizeof(filters)/sizeof(filters[0]);

/**
 * struct b2r2_filter_spec bilinear_filter - A bilinear filter
 *
 * The bilinear filter will be used if no custom filters are specified, or
 * for upscales not matching any filter in the lookup table.
 */
static struct b2r2_filter_spec bilinear_filter = {
	.min = 0,
	.max = 0xffff,
	.h_coeffs = {
		0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xff, 0x08, 0x3e, 0xfb, 0x00, 0x00,
		0x00, 0x00, 0xfb, 0x13, 0x3b, 0xf7, 0x00, 0x00,
		0x00, 0x00, 0xf8, 0x1f, 0x34, 0xf5, 0x00, 0x00,
		0x00, 0x00, 0xf6, 0x2b, 0x2a, 0xf5, 0x00, 0x00,
		0x00, 0x00, 0xf6, 0x35, 0x1e, 0xf7, 0x00, 0x00,
		0x00, 0x00, 0xf9, 0x3c, 0x12, 0xf9, 0x00, 0x00,
		0x00, 0x00, 0xfd, 0x3f, 0x07, 0xfd, 0x00, 0x00
	},
	.v_coeffs = {
		0x00, 0x00, 0x40, 0x00, 0x00,
		0x00, 0x09, 0x3d, 0xfa, 0x00,
		0x00, 0x13, 0x39, 0xf4, 0x00,
		0x00, 0x1e, 0x31, 0xf1, 0x00,
		0x00, 0x27, 0x28, 0xf1, 0x00,
		0x00, 0x31, 0x1d, 0xf2, 0x00,
		0x00, 0x38, 0x12, 0xf6, 0x00,
		0x00, 0x3d, 0x07, 0xfc, 0x00
	},
};

/**
 * struct b2r2_filter_spec default_downscale_filter - Default filter for downscale
 *
 * The default downscale filter will be used for downscales not matching any
 * filter in the lookup table.
 */
static struct b2r2_filter_spec default_downscale_filter = {
	.min = 1 << 10,
	.max = 0xffff,
	.h_coeffs = {
		0x03, 0x06, 0x09, 0x0b, 0x09, 0x0b, 0x09, 0x06,
		0x03, 0x06, 0x09, 0x0b, 0x0c, 0x0a, 0x08, 0x05,
		0x03, 0x06, 0x09, 0x0b, 0x0c, 0x0a, 0x08, 0x05,
		0x04, 0x07, 0x09, 0x0b, 0x0b, 0x0a, 0x08, 0x04,
		0x04, 0x07, 0x0a, 0x0b, 0x0b, 0x0a, 0x07, 0x04,
		0x04, 0x08, 0x0a, 0x0b, 0x0b, 0x09, 0x07, 0x04,
		0x05, 0x08, 0x0a, 0x0b, 0x0c, 0x09, 0x06, 0x03,
		0x05, 0x08, 0x0a, 0x0b, 0x0c, 0x09, 0x06, 0x03
	},
	.v_coeffs = {
		0x0b, 0x0e, 0x0e, 0x0e, 0x0b,
		0x0b, 0x0e, 0x0f, 0x0d, 0x0b,
		0x0c, 0x0e, 0x0f, 0x0d, 0x0a,
		0x0c, 0x0e, 0x0f, 0x0d, 0x0a,
		0x0d, 0x0f, 0x0e, 0x0d, 0x09,
		0x0d, 0x0f, 0x0f, 0x0c, 0x09,
		0x0e, 0x0f, 0x0e, 0x0c, 0x09,
		0x0e, 0x0f, 0x0f, 0x0c, 0x08
	},
};

/**
 * struct b2r2_filter_spec blur_filter - Blur filter
 *
 * Filter for blurring an image.
 */
static struct b2r2_filter_spec blur_filter = {
	.min = 0,
	.max = 0xffff,
	.h_coeffs = {
		0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
		0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
		0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
		0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
		0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
		0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
		0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
		0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08
	},
	.v_coeffs = {
		0x0c, 0x0c, 0x10, 0x0c, 0x0c,
		0x0c, 0x0c, 0x10, 0x0c, 0x0c,
		0x0c, 0x0c, 0x10, 0x0c, 0x0c,
		0x0c, 0x0c, 0x10, 0x0c, 0x0c,
		0x0c, 0x0c, 0x10, 0x0c, 0x0c,
		0x0c, 0x0c, 0x10, 0x0c, 0x0c,
		0x0c, 0x0c, 0x10, 0x0c, 0x0c,
		0x0c, 0x0c, 0x10, 0x0c, 0x0c
	},
};

/* Private function declarations */
static int alloc_filter_coeffs(struct device *dev,
		struct b2r2_filter_spec *filter);
static void free_filter_coeffs(struct device *dev,
		struct b2r2_filter_spec *filter);

/* Public functions */

int b2r2_filters_init(struct b2r2_control *cont)
{
	int i;

	if (cont->filters_initialized)
		return 0;

	for (i = 0; i < filters_size; i++) {
		alloc_filter_coeffs(cont->dev, &filters[i]);
	}

	alloc_filter_coeffs(cont->dev, &bilinear_filter);
	alloc_filter_coeffs(cont->dev, &default_downscale_filter);
	alloc_filter_coeffs(cont->dev, &blur_filter);

	cont->filters_initialized = 1;

	return 0;
}

void b2r2_filters_exit(struct b2r2_control *cont)
{
	int i;

	if (!cont->filters_initialized)
		return;

	for (i = 0; i < filters_size; i++) {
		free_filter_coeffs(cont->dev, &filters[i]);
	}

	free_filter_coeffs(cont->dev, &bilinear_filter);
	free_filter_coeffs(cont->dev, &default_downscale_filter);
	free_filter_coeffs(cont->dev, &blur_filter);

	cont->filters_initialized = 0;
}

struct b2r2_filter_spec *b2r2_filter_find(u16 scale_factor)
{
	int i;
	struct b2r2_filter_spec *filter = NULL;

	for (i = 0; i < filters_size; i++) {
		if ((filters[i].min < scale_factor) &&
				(scale_factor <= filters[i].max) &&
				filters[i].h_coeffs_dma_addr &&
				filters[i].v_coeffs_dma_addr) {
			filter = &filters[i];
			break;
		}
	}

	if (filter == NULL) {
		/*
		 * No suitable filter has been found. Use default filters,
		 * bilinear for any upscale.
		 */
		if (scale_factor < (1 << 10))
			filter = &bilinear_filter;
		else
			filter = &default_downscale_filter;
	}

	/*
	 * Check so that the coefficients were successfully allocated for this
	 * filter.
	 */
	if (!filter->h_coeffs_dma_addr || !filter->v_coeffs_dma_addr)
		return NULL;
	else
		return filter;
}

struct b2r2_filter_spec *b2r2_filter_blur()
{
	return &blur_filter;
}

/* Private functions */
static int alloc_filter_coeffs(struct device *dev,
		struct b2r2_filter_spec *filter)
{
	int ret;

	filter->h_coeffs_dma_addr = dma_alloc_coherent(dev,
			B2R2_HF_TABLE_SIZE, &(filter->h_coeffs_phys_addr),
			GFP_DMA | GFP_KERNEL);
	if (filter->h_coeffs_dma_addr == NULL) {
		ret = -ENOMEM;
		goto error;
	}

	filter->v_coeffs_dma_addr = dma_alloc_coherent(dev,
			B2R2_VF_TABLE_SIZE, &(filter->v_coeffs_phys_addr),
			GFP_DMA | GFP_KERNEL);
	if (filter->v_coeffs_dma_addr == NULL) {
		ret = -ENOMEM;
		goto error;
	}

	memcpy(filter->h_coeffs_dma_addr, filter->h_coeffs,
			B2R2_HF_TABLE_SIZE);
	memcpy(filter->v_coeffs_dma_addr, filter->v_coeffs,
			B2R2_VF_TABLE_SIZE);

	return 0;

error:
	free_filter_coeffs(dev, filter);
	return ret;

}

static void free_filter_coeffs(struct device *dev,
		struct b2r2_filter_spec *filter)
{
	if (filter->h_coeffs_dma_addr != NULL)
		dma_free_coherent(dev, B2R2_HF_TABLE_SIZE,
				filter->h_coeffs_dma_addr,
				filter->h_coeffs_phys_addr);
	if (filter->v_coeffs_dma_addr != NULL)
		dma_free_coherent(dev, B2R2_VF_TABLE_SIZE,
				filter->v_coeffs_dma_addr,
				filter->v_coeffs_phys_addr);

	filter->h_coeffs_dma_addr = NULL;
	filter->h_coeffs_phys_addr = 0;
	filter->v_coeffs_dma_addr = NULL;
	filter->v_coeffs_phys_addr = 0;
}