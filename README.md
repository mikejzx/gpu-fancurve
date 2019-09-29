# linux-nvidia-fan-curve
This simple program will adjust the GPU's fan speed based on the temperature of it. A configuration file is used to determine the "points" at which the fan speeds should be.
<br>
Note that only <b>a single GPU</b> is supported. This will not adjust speeds of any but the first GPU in the computer.
<br>
Also, the command below likely must be run, along with a reboot in order for the program to function:
`sudo nvidia-xconfig -a --cool-bits=28 --allow-empty-initial-configuration`
<br>
Of course I could've just used one of the many Shell scripts on the net, but that would've been too easy...
