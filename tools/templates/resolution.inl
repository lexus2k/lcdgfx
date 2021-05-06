
static const PROGMEM uint8_t s_~CONTROLLER~_lcd~RESOLUTION~_initData[] = {
~INIT~
};

////////////////////////////////////////////////////////////////////////////////
//             ~CONTROLLER~ basic ~BITS~-bit implementation
////////////////////////////////////////////////////////////////////////////////

template <class I> void Display~CONTROLLER~_~RESOLUTION~<I>::begin()
{
    ssd1306_resetController2(this->m_rstPin, ~RESET_DURATION~);
    this->m_w = ~WIDTH~;
    this->m_h = ~HEIGHT~;
    // Give LCD some time to initialize. Refer to ~CONTROLLER~ datasheet
    lcd_delay(~RESET_DELAY~);
    ~CONFIG_FUNC~<I>(this->m_intf,
                            s_~CONTROLLER~_lcd~RESOLUTION~_initData,
                            sizeof(s_~CONTROLLER~_lcd~RESOLUTION~_initData));
~OPTIONAL_CONFIG~
}

template <class I> void Display~CONTROLLER~_~RESOLUTION~<I>::end()
{
}
