void Display~CONTROLLER~_~RESOLUTION~_I2C::begin()
{
    m_i2c.begin();
    Display~CONTROLLER~_~RESOLUTION~::beginController();
}

void Display~CONTROLLER~_~RESOLUTION~_I2C::end()
{
    Display~CONTROLLER~_~RESOLUTION~::endController();
    m_i2c.end();
}
