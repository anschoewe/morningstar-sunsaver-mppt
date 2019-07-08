# Getting Started
Before compiling code, make sure you have the correct path to your MODBUS adapter.  On my Mac it was `/dev/tty.usbserial-DO001BL8`.
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

# Save output
Run the following command and then press '1'

`./sunsaver > output/snapshot_$(date +%Y-%m-%d_%H-%M-%S)`

Run the following command and then press '3'

`./sunsaver > output/logs_$(date +%Y-%m-%d_%H-%M-%S)`

# Commit Changes
TODO

# Custom Programming Values
These suggestions are based on my [battery specifications](https://www.batterystuff.com/files/376-45825.pdf).  This is a 55Ah AGM sealed battery. I have two of these wired in parallel for a total voltage of 12V and a total capacity of 110Ah.
- **EV_reg**: The voltage used during the main charging phase (sometimes called 'absorption', 'regulation' or 'control' voltage).  I purposely set this high so that we get a full charge.  If you don't fully charge the battery than the capacity diminishes a bit after every charge cycle.
  - **14.8V**
- **EV_float**: The voltage used during the float/trickle charging phase.  I've left this at the default 13.7V.

# Understanding Logs
The logs are reported in a somewhat complicated fashion. The logs are stored in a ring-buffer.  After printing the log results, you should find the record with the smallest 'hourmeter' value.  This corresponds to the oldest log in the ring buffer.  The records are then stored sequentially (and potentially loop around to the first few records printed.  Typically, you should expect 32 log records which represents the last 32 days of operation.
- **hourmeter**: The number of hours that the charger has been in operation.  There is typically a log record every 24 hours.  This allows you to determine when you started using the solar charger.  In my case it was on Oct. 1st, 2018, around 6pm EST.
- **Vb_min_daily**: The minimum battery voltage recorded during the 24 hour period.  
- **Vb_max_daily**: The maximum battery voltage recorded during the 24 hour period.
- ### **Ahc_daily**: The number of amp-hours charged/sent to the battery during the 24 hours period.
- **Ahl_daily**: The number of amp-hours sent out to the 'load' during the 24 hours period. If your load pulls directly from the battery instead of the solar charger, you should expect this to be zero.
- **Va_max_daily**: The maximum solar panel voltage recorded as it came into the solar charger duinr the 24 hour period.
- **time_ab_daily**: The number of minutes spent in the 'absorption' battery charge phase during the 24 hours period.
- **time_eq_daily**: The number of minutes spent in the 'equalize' battery charge phase during the 24 hours period.  Since I'm using an AGM battery, I disabled battery equalization, so I expect this vaulue to be zero.
- **time_fl_daily**: The number of minutes spent in the 'float' battery charge phase during the 24 hours period.

# Helpful Links
- Morningstar official documentation: http://support.morningstarcorp.com/wp-content/uploads/2014/07/SSMPPT.APP_Public-Modbus-Doc_EN_v11.pdf 
- libmodbus documentation: http://libmodbus.org/docs/v3.1.4/modbus_write_register.html
- Example blog showing example of how to connect to MorningStar SunSaver using libmodbus on Mac/Linux: https://westyd1982.wordpress.com/2010/03/26/linux-and-mac-os-x-software-to-read-data-from-the-sunsaver-mppt-using-modbus/

