/**
 * Class implements basic functions for ~BITS~-bit mode of ~CONTROLLER~-based displays
 */
template <class I> class Display~CONTROLLER~_~RESOLUTION~: public Display~CONTROLLER~~EXBITS~<I>
{
public:
    /**
     * Creates instance of ~CONTROLLER~ ~RESOLUTION~ controller class for ~BITS~-bit mode
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     */
    Display~CONTROLLER~_~RESOLUTION~(I &intf, int8_t rstPin)
        : Display~CONTROLLER~~EXBITS~<I>(intf, rstPin)
    {
    }

protected:
    /**
     * Basic ~CONTROLLER~ ~RESOLUTION~ initialization
     */
    void begin() override;

    /**
     * Basic ~CONTROLLER~ deinitialization
     */
    void end() override;
};
