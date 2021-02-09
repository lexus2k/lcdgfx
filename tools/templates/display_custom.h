/**
 * Template class implements ~CONTROLLER~ ~RESOLUTION~ lcd display in ~BITS~ bit mode over custom interface implementation
 * (user-defined interface implementation). I - user custom interface class.
 * This class allows to use display over 6800 and 8080 parallel interfaces
 */
template <class I> class Display~CONTROLLER~_~RESOLUTION~_Custom: public Display~CONTROLLER~_~RESOLUTION~<Interface~CONTROLLER~<I>>
{
public:
    /**
     * @brief Inits ~RESOLUTION~ lcd display over custom interface (based on ~CONTROLLER~ controller): ~BITS~-bit mode.
     *
     * Inits ~RESOLUTION~ lcd display over custom interface (based on ~CONTROLLER~ controller): ~BITS~-bit mode
     * User-defined interface `I` will receive frequency (uint32_t) as first argument, and user additional args,
     * specified as data variable argument list.
     *
     * @param rstPin pin controlling LCD reset (-1 if not used)
     * @param dcPin pin to use as data/command control pin (-1 if not used)
     * @param frequency frequency in HZ, or 0 if to use default display frequency
     * @param data variable argument list for custom user interface.
     */
    template <typename... Args>
    Display~CONTROLLER~_~RESOLUTION~_Custom(int8_t rstPin, int8_t dcPin, uint32_t frequency, Args &&... data)
        : Display~CONTROLLER~_~RESOLUTION~<Interface~CONTROLLER~<I>>(m_custom, rstPin)
        , m_custom(~CUSTOM_INTERFACE_ARGS~, data...)
    {
    }

    /**
     * Initializes ~CONTROLLER~ lcd in ~BITS~-bit mode
     */
    void begin() override
    {
        m_custom.begin();
        Display~CONTROLLER~_~RESOLUTION~<Interface~CONTROLLER~<I>>::begin();
    }

    /**
     * Closes connection to display
     */
    void end() override
    {
        Display~CONTROLLER~_~RESOLUTION~<Interface~CONTROLLER~<I>>::end();
        m_custom.end();
    }

private:
    Interface~CONTROLLER~<I> m_custom;
};
