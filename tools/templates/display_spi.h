/**
 * Class implements ~CONTROLLER~ ~RESOLUTION~ lcd display in ~BITS~ bit mode over SPI
 */
class Display~CONTROLLER~_~RESOLUTION~_SPI: public Display~CONTROLLER~_~RESOLUTION~<Interface~CONTROLLER~<PlatformSpi>>
{
public:
    /**
     * @brief Inits ~RESOLUTION~ lcd display over spi (based on ~CONTROLLER~ controller): ~BITS~-bit mode.
     *
     * Inits ~RESOLUTION~ lcd display over spi (based on ~CONTROLLER~ controller): ~BITS~-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param config platform spi configuration. Please refer to SPlatformSpiConfig.
     */
    explicit Display~CONTROLLER~_~RESOLUTION~_SPI(int8_t rstPin, const SPlatformSpiConfig &config = {-1, {-1}, -1, 0, -1, -1})
        : Display~CONTROLLER~_~RESOLUTION~(m_spi, rstPin)
        , m_spi(~SERIAL_INTERFACE_ARGS~,
                SPlatformSpiConfig{
                    config.busId, {config.cs}, config.dc, config.frequency ?: ~FREQUENCY~, config.scl, config.sda})
    {
    }

    /**
     * Initializes ~CONTROLLER~ lcd in ~BITS~-bit mode
     */
    void begin() override;

    /**
     * Closes connection to display
     */
    void end() override;

private:
    Interface~CONTROLLER~<PlatformSpi> m_spi;
};

/**
 * Template class implements ~CONTROLLER~ ~RESOLUTION~ lcd display in ~BITS~ bit mode over custom SPI implementation
 * (user-defined spi implementation). I - user custom spi class
 */
template <class I> class Display~CONTROLLER~_~RESOLUTION~_CustomSPI: public Display~CONTROLLER~_~RESOLUTION~<Interface~CONTROLLER~<I>>
{
public:
    /**
     * @brief Inits ~RESOLUTION~ lcd display over spi (based on ~CONTROLLER~ controller): ~BITS~-bit mode.
     *
     * Inits ~RESOLUTION~ lcd display over spi (based on ~CONTROLLER~ controller): ~BITS~-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param dcPin pin to use as data/command control pin
     * @param data variable argument list for custom user spi interface.
     */
    template <typename... Args>
    Display~CONTROLLER~_~RESOLUTION~_CustomSPI(int8_t rstPin, int8_t dcPin, Args &&... data)
        : Display~CONTROLLER~_~RESOLUTION~<Interface~CONTROLLER~<I>>(m_spi, rstPin)
        , m_spi(~CUSTOM_SERIAL_INTERFACE_ARGS~, data...)
    {
    }

    /**
     * Initializes ~CONTROLLER~ lcd in ~BITS~-bit mode
     */
    void begin() override
    {
        m_spi.begin();
        Display~CONTROLLER~_~RESOLUTION~<Interface~CONTROLLER~<I>>::begin();
    }

    /**
     * Closes connection to display
     */
    void end() override
    {
        Display~CONTROLLER~_~RESOLUTION~<Interface~CONTROLLER~<I>>::end();
        m_spi.end();
    }

private:
    Interface~CONTROLLER~<I> m_spi;
};
