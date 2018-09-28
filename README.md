# Getting Started
Before compiling code, make sure you have the correct path to your MODBUS adapter.  On my Mac it was /dev/tty.usbserial-DO001BL8

# Compile code with...
cc `pkg-config --cflags --libs libmodbus` sunsaver.c -o sunsaver && chmod +x sunsaver

# Run
`./sunsaver`

# Commit Changes
TODO

# Helpful Links
- Morningstar official documentation: http://support.morningstarcorp.com/wp-content/uploads/2014/07/SSMPPT.APP_Public-Modbus-Doc_EN_v11.pdf 
- libmodbus documentation: http://libmodbus.org/docs/v3.1.4/modbus_write_register.html
- Example blog showing example of how to connect to MorningStar SureSine using libmodbus on Mac/Linux: https://westyd1982.wordpress.com/2010/03/26/linux-and-mac-os-x-software-to-read-data-from-the-sunsaver-mppt-using-modbus/

