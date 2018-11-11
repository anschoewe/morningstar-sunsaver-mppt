# Getting Started
Before compiling code, make sure you have the correct path to your MODBUS adapter.  On my Mac it was /dev/tty.usbserial-DO001BL8
Also, make sure you have libmodbus installed.

# Install libmodbus on Mac
Using homebrew...

```
brew update
brew install libmodbus
```

# Compile code with...

```
g++ `pkg-config --cflags --libs libmodbus` sunsaver.cpp -o sunsaver && chmod +x sunsaver
```

# Run
`./sunsaver`

# Commit Changes
TODO

# Understanding Logs
The logs are reported in a somewhat complicated fashion. The logs are stored in a ring-buffer.  After printing the log results, you should find the record with the smallest 'hourmeter' value.  This corresponds to the oldest log in the ring buffer.  The records are then stored sequentially (and potentially loop around to the first few records printed.  Typically, you should expect 32 log records which represents the last 32 days of operation.
- *hourmeter*: The number of hours that the charger has been in operation.  There is typically a log record every 24 hours.  This allows you to determine when you started using the solar charger.  In my case it was on Oct. 1st, 2018, around 6pm EST.
- *Vb_min_daily*: Minimum battery voltage recorded during the 24 hour period.  

# Helpful Links
- Morningstar official documentation: http://support.morningstarcorp.com/wp-content/uploads/2014/07/SSMPPT.APP_Public-Modbus-Doc_EN_v11.pdf 
- libmodbus documentation: http://libmodbus.org/docs/v3.1.4/modbus_write_register.html
- Example blog showing example of how to connect to MorningStar SunSaver using libmodbus on Mac/Linux: https://westyd1982.wordpress.com/2010/03/26/linux-and-mac-os-x-software-to-read-data-from-the-sunsaver-mppt-using-modbus/

