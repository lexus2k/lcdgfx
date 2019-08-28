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
     * @param config platform spi configuration. Please refer to SPlatformI2cConfig.
     */
    Display~CONTROLLER~_~RESOLUTION~_SPI( int8_t rstPin, const SPlatformSpiConfig &config = { -1, -1, -1, 0, -1, -1 } )
        : Display~CONTROLLER~_~RESOLUTION~(m_spi, rstPin)
        , m_spi( ~INTERFACE_ARGS~,
                 SPlatformSpiConfig{ config.busId,
                                     config.cs,
                                     config.dc,
                                     config.frequency ?: ~FREQUENCY~,
                                     config.scl,
                                     config.sda } ) {}

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

