#!/bin/bash
echo "This script will copy the config file to ~/.config/gpu-fancurve.conf, copy the program to /usr/local/bin, and set it to run automatically when you logon."
echo
echo "**sudo rights are required and you will be prompted for your sudo password**  If you just want to run gpu-fancurve manually or don't have sudo rights, please see README.md"
echo

#Check if running as root, abort if so because the config file won't be in the right path
if [ "`id -u`" == "0" ]
then
   echo "You appear to be running this script as root.  Please run this script as your regular user account instead and you will be prompted for your sudo password as needed.  Cancelling install, no changes have been made to the system."
   exit
fi

#check for nvidia-xconfig in path
which nvidia-xconfig > /dev/null 2>&1
if [ $? != 0 ]
then
   echo "Couldn't find nvidia-xconfig in path.  Are the proprietary Nvidia drivers installed?"
   echo "Cancelling install, no changes have been made to the system."
   exit
fi

#Get user confirmation
while true; do
   read -p "Do you want to continue with the install? (YES/no) " prompt
   if [ "${prompt,,}" == "yes" ] || [ "${prompt,,}" == "y" ] || [ "${prompt,,}" == "" ]
   then
      break
   elif [ "${prompt,,}" == "no" ] || [ "${prompt,,}" == "n" ]
   then
      echo "Cancelling install, no changes have been made to the system."
      exit
   fi
   echo "Sorry, I didn't understand that. Please type yes or no"
done

#Look for the binary and if it isn't there, compile it.  Once we have a binary, copy it to /usr/local/bin
if ! [ -f "gpu-fancurve" ]
then
   echo "Couldn't find the executable, attempting to compile from source..."
   g++ fan.cpp -o ./gpu-fancurve -std=c++17
   if ! [ -f "gpu-fancurve" ]
   then
      echo "Compile failed, cancelling install."
      exit
   fi
   chmod +x gpu-fancurve
fi
#Attempt to kill the process before the copy, just in case it is running
pkill gpu-fancurve
echo "Copying gpu-fancurve to /usr/local/bin..."
sudo cp gpu-fancurve /usr/local/bin/

#Check to make sure gpu-fancurve and /usr/local/bin/gpu-fancurve are identical
cmp -s gpu-fancurve /usr/local/bin/gpu-fancurve
if [ $? != 0 ]
then
   echo "Copy seems to have failed, cancelling install."
   exit
fi

#Look for gpu-fancurve.conf and copy it to ~/.config/gpu-fancurve.conf
#If it doesn't exist, ask the user if they want to use the example config
echo "Copying gpu-fancurve.conf to ~/.config/gpu-fancurve.conf..."
if [ -f "gpu-fancurve.conf" ]
then
   cp gpu-fancurve.conf ~/.config/
else
   while true; do
      read -p "Couldn't find gpu-fancurve.conf.  Use example configuration? (YES/no) " prompt
      if [ "${prompt,,}" == "yes" ] || [ "${prompt,,}" == "y" ] || [ "${prompt,,}" == "" ]
      then
         cp gpu-fancurve.conf.example ~/.config/gpu-fancurve.conf
         break
      elif [ "${prompt,,}" == "no" ] || [ "${prompt,,}" == "n" ]
      then
         echo "You will need to setup a config file at ~/.config/gpu-fancurve.conf before gpu-fancurve can run automatically."
         echo
         break
      fi
      echo "Sorry, I didn't understand that. Please type yes or no"
   done
fi
if ! [ -f "~/.config/gpu-fancurve.conf" ]
then
   echo "Couldn't create ~/.config/gpu-fancurve.conf.  Please create the file manually using the example file as a guide."
fi

#Setup gpu-fancurve to run on logon
echo "Creating ~/.config/autostart/gpu-fancurve.desktop so gpu-fancurve will start at logon"
cat << EOF > ~/.config/autostart/gpu-fancurve.desktop
[Desktop Entry]
Type=Application
Name=gpu-fancurve
Exec=gpu-fancurve
EOF

echo "Setting the cool-bits flag in the Nvidia drivers..."
sudo nvidia-xconfig -a --cool-bits=28 --allow-empty-initial-configuration

echo "Done!  Please reboot for the cool-bits change and to allow gpu-fancurve to autostart."

