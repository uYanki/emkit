#ifndef EGL_DEVICE_ST7735_H
#define EGL_DEVICE_ST7735_H

#include "../egl_device.h"
#include "spi_st7735.h"

EGL_NAMESPACE_BEGIN

class graphics_device_st7735;

class graphics_device_st7735 : public graphics_device<graphics_device_st7735> {
public:
    // device properties

    enum {
        fb_width  = ST7735_WIDTH,
        fb_height = ST7735_HEIGHT,
    };

    enum { fb_format = pixfmt_b5g6r5 };

    typedef pixel<e_pixel_format(fb_format)> fb_format_t;

    // construction

    graphics_device_st7735();
    ~graphics_device_st7735();

    void init(void* device);
    void init_rasterizer(const rasterizer_cfg&, const rasterizer_tiling_cfg&, const rasterizer_vertex_cache_cfg&);
    void init_dma(rasterizer_data_transfer*, uint8_t num_transfers_, fb_format_t* dma_buffer_, size_t dma_buffer_size_);

    // immediate rendering interface

    template <class IPShader>
    EGL_INLINE void fast_draw_hline(uint16_t x_, uint16_t y_, uint16_t width_, const IPShader&);
    template <class IPShader>
    EGL_INLINE void fast_draw_vline(uint16_t x_, uint16_t y_, uint16_t height_, const IPShader&);
    template <class IPShader>
    EGL_INLINE void fast_draw_rect(uint16_t x_, uint16_t y_, uint16_t width_, uint16_t height_, const IPShader&);

private:
    graphics_device_st7735(const graphics_device_st7735&);  // not implemented
    void         operator=(const graphics_device_st7735&);  // not implemented
    virtual void submit_tile(uint8_t tx_, uint8_t ty_, const vec2u16& reg_min_, const vec2u16& reg_end_, uint16_t thread_idx_);

    static graphics_device_st7735* s_active_dev;

    // spi
    spi_st7735_t* m_device;

    // tile data
    fb_format_t*           m_tile_rt0;
    rasterizer_tile_size_t m_tile_width;
    rasterizer_tile_size_t m_tile_height;
};

template <class IPShader>
void graphics_device_st7735::fast_draw_hline(uint16_t x_, uint16_t y_, uint16_t width_, const IPShader& ips_)
{
    // draw horizontal line

    uint16_t x_end = x_ + width_;

    if (ST7735_StartDraw(m_device, x_, y_, width_, 1) == true)
    {
        fb_format_t res;

        do {
            ips_.exec(res, x_, y_);
            ST7735_PutColor(m_device, res.v);
        } while (++x_ < x_end);

        ST7735_StopDraw(m_device);
    }
}

template <class IPShader>
void graphics_device_st7735::fast_draw_vline(uint16_t x_, uint16_t y_, uint16_t height_, const IPShader& ips_)
{
    // draw vertical line

    uint16_t y_end = y_ + height_;

    if (ST7735_StartDraw(m_device, x_, y_, 1, height_) == true)
    {
        fb_format_t res;

        do {
            ips_.exec(res, x_, y_);
            ST7735_PutColor(m_device, res.v);
        } while (++y_ < y_end);

        ST7735_StopDraw(m_device);
    }
}

template <class IPShader>
void graphics_device_st7735::fast_draw_rect(uint16_t x_, uint16_t y_, uint16_t width_, uint16_t height_, const IPShader& ips_)
{
    // draw rectangle
    uint16_t x_start = x_;
    uint16_t x_end   = x_ + width_;
    uint16_t y_end   = y_ + height_;

    if (ST7735_StartDraw(m_device, x_, y_, width_, height_) == true)
    {
        fb_format_t res;

        do {
            x_ = x_start;

            do
            {
                ips_.exec(res, x_, y_);
                ST7735_PutColor(m_device, res.v);

            } while (++x_ < x_end);

        } while (++y_ < y_end);

        ST7735_StopDraw(m_device);
    }
}

EGL_NAMESPACE_END

#endif
