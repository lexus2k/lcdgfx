/**
 * Class implements ~CONTROLLER~ ~RESOLUTION~ lcd display in ~BITS~ bit mode over I2C
 */
class Display~CONTROLLER~_~RESOLUTION~_I2C: public Display~CONTROLLER~_~RESOLUTION~<Interface~CONTROLLER~<PlatformI2c>>
{
public:
    /**
     * @brief Inits ~RESOLUTION~ lcd display over i2c (based on ~CONTROLLER~ controller): ~BITS~-bit mode.
     *
     * Inits ~RESOLUTION~ lcd display over i2c (based on ~CONTROLLER~ controller): ~BITS~-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param config platform i2c configuration. Please refer to SPlatformI2cConfig.
     */
    explicit Display~CONTROLLER~_~RESOLUTION~_I2C(int8_t rstPin, const SPlatformI2cConfig &config = {-1, ~I2C_ADDR~, -1, -1, 0})
        : Display~CONTROLLER~_~RESOLUTION~(m_i2c, rstPin)
        , m_i2c(~SERIAL_INTERFACE_ARGS~,
                SPlatformI2cConfig{config.busId, static_cast<uint8_t>(config.addr ?: ~I2C_ADDR~), config.scl, config.sda,
                                   config.frequency ?: 400000})
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
    Interface~CONTROLLER~<PlatformI2c> m_i2c;
};

/**
 * Template class implements ~CONTROLLER~ ~RESOLUTION~ lcd display in ~BITS~ bit mode over custom I2C implementation
 * (user-defined i2c implementation). I - user custom i2c class
 */
template <class I> class Display~CONTROLLER~_~RESOLUTION~_CustomI2C: public Display~CONTROLLER~_~RESOLUTION~<Interface~CONTROLLER~<I>>
{
public:
    /**
     * @brief Inits ~RESOLUTION~ lcd display over i2c (based on ~CONTROLLER~ controller): ~BITS~-bit mode.
     *
     * Inits ~RESOLUTION~ lcd display over i2c (based on ~CONTROLLER~ controller): ~BITS~-bit mode
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param data variable argument list for custom user i2c interface.
     */
    template <typename... Args>
    Display~CONTROLLER~_~RESOLUTION~_CustomI2C(int8_t rstPin, Args &&... data)
        : Display~CONTROLLER~_~RESOLUTION~<Interface~CONTROLLER~<I>>(m_i2c, rstPin)
        , m_i2c(~SERIAL_INTERFACE_ARGS~, data...)
    {
    }

    /**
     * Initializes ~CONTROLLER~ lcd in ~BITS~-bit mode
     */
    void begin() override
    {
        m_i2c.begin();
        Display~CONTROLLER~_~RESOLUTION~<Interface~CONTROLLER~<I>>::begin();
    }

    /**
     * Closes connection to display
     */
    void end() override
    {
        Display~CONTROLLER~_~RESOLUTION~<Interface~CONTROLLER~<I>>::end();
        m_i2c.end();
    }

private:
    Interface~CONTROLLER~<I> m_i2c;
};
