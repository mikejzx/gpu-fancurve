# gpu-fancurve (for NVIDIA cards)
This simple program adjusts an NVIDIA GPU's fan speed based on its temperature.
A configuration file is used to define points in the fan speed curve.

Note that this program supports only <b>a single GPU</b>.
Therefore the program will only adjust the fan speeds of the <i>first GPU</i> that it finds in the computer.

To customize the fan curve, copy gpu-fancurve.conf.example to gpu-fancurve.conf.
Please ensure the temperatures in the curve points <i>rise as it approaches the end of the file.</i>

Run `make install` to setup the program in /usr/local/bin and set it to start automatically when you log in.
If there is a gpu-fancurve.conf file in the same folder as the install.sh script then it will use that, otherwise you will be asked if you want to use the example config.

If you want to update the curve, make the changes in gpu-fancurve.conf and run `make update_config`

## Manual execution

### Prerequisites

The command below must be run, along with a reboot in order for the program to function:

`sudo nvidia-xconfig -a --cool-bits=4 --allow-empty-initial-configuration`

### Config file

The configuration file is NOT created by the program, the user must create it manually.<br>
The default path of this file is at `~/.config/gpu-fancurve.conf`.
An example configuration file is provided with the source code.

### Arguments
<ul>
<li>
  -f, --config-path<br>
  Specifies the location of the configuration path.<br>
  By default the program checks `~/.config/gpu-fancurve.conf`. (See 'Config file' section above for more information.)
</li>

<li>
  -v, --verbose<br>
  Increase verbosity; i.e: show the temperature and fan speed as it changes.
</li>

<li>-h, --help<br>
  Displays help information.
</li>
</ul>
