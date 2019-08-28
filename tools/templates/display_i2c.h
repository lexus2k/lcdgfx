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
    Display~CONTROLLER~_~RESOLUTION~_I2C( int8_t rstPin, const SPlatformI2cConfig &config = { -1, ~I2C_ADDR~, -1, -1, 0 } )
        : Display~CONTROLLER~_~RESOLUTION~(m_i2c, rstPin)
        , m_i2c( ~INTERFACE_ARGS~,
                 SPlatformI2cConfig{ config.busId,
                                     config.addr ?: (uint8_t)~I2C_ADDR~,
                                     config.scl,
                                     config.sda,
                                     config.frequency ?: 400000 } ) {}

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

