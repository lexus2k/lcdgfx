void Display~CONTROLLER~_~RESOLUTION~_SPI::begin()
{
    m_spi.begin();
    Display~CONTROLLER~_~RESOLUTION~::begin();
}

void Display~CONTROLLER~_~RESOLUTION~_SPI::end()
{
    Display~CONTROLLER~_~RESOLUTION~::end();
    m_spi.end();
}
