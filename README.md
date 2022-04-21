# gpu-fancurve
This simple program will adjust the GPU's fan speed based on its temperature. A configuration file is used to determine the "points" at which the fan speeds should be.<br>
<br>
Note that only <b>a single GPU</b> is supported. This will not adjust speeds of any but the first GPU in the computer.<br>
<br>
To customize the fan curve, copy gpu-fancurve.conf.example to gpu-fancurve.conf.  Please ensure the temperatures in the curve points <i>rise as it approaches the end of the file.</i><br>
<br>
Run install.sh to setup the program in /usr/local/bin and set it to start automatically when you log in.  If there is a gpu-fancurve.conf file in the same folder as install.sh it will use that, otherwise you will be asked if you want to use the example config.<br>
<br>
If you want to update the curve, make the changes in gpu-fancurve.conf and run update-config.sh<br>
<br>
<h6>Of course I could've just used one of the many shell scripts on the net, but that would've been too easy...</h6>

## Manual execution
0.0: PREREQUISITES:
The command below must be run, along with a reboot in order for the program to function:<br>
`sudo nvidia-xconfig -a --cool-bits=4 --allow-empty-initial-configuration`

1.0: CONFIG FILE:<br>
The configuration file is NOT created by the program, the user must create it manually.<br>
The default path of this file is at `~/.config/gpu-fancurve.conf`. An example configuration file is provided.<br>
<br>
2.0: ARGUMENTS:<br>
<ul>
<li>-f, --config-path<br>
Specifies the location of the configuration path.<br>
By default the program checks `~/.config/gpu-fancurve.conf`. (See '1.0: CONFIG FILE', above for more information.)<br>
  <br></li>
<li>
  -v, --verbose<br>
  Increase verbosity; i.e: show the temperature and fan speed as it changes.<br>
  <br></li>
<li>-h, --help<br>
  Displays this help information.<br></li>
  </ul>
