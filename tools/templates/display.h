/**
 * Class implements basic functions for ~BITS~-bit mode of ~CONTROLLER~-based displays
 */
template <typename I> class Display~CONTROLLER~~EXBITS~: public NanoDisplayOps<NanoDisplayOps~BITS~<I>, I>
{
public:
    /**
     * Creates instance of ~CONTROLLER~ controller class for ~BITS~-bit mode
     *
     * @param intf interface to use
     * @param rstPin pin to use as HW reset pin for LCD display
     */
    Display~CONTROLLER~~EXBITS~(I &intf, int8_t rstPin)
        : NanoDisplayOps<NanoDisplayOps~BITS~<I>, I>(intf)
        , m_rstPin(rstPin)
    {
    }

protected:
    int8_t m_rstPin; ///< indicates hardware reset pin used, -1 if it is not required

    /**
     * Basic ~CONTROLLER~ initialization
     */
    void beginDisplay();

    /**
     * Basic ~CONTROLLER~ deinitialization
     */
    void endDisplay();
};

