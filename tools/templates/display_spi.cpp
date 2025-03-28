void Display~CONTROLLER~_~RESOLUTION~_SPI::begin()
{
    m_spi.begin();
    Display~CONTROLLER~_~RESOLUTION~::beginController();
}

void Display~CONTROLLER~_~RESOLUTION~_SPI::end()
{
    Display~CONTROLLER~_~RESOLUTION~::endController();
    m_spi.end();
}
