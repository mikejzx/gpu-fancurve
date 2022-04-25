#!/bin/bash
echo "This script will stop any running instances of gpu-fancurve and remove:"
echo "-The config file from $HOME/.config/gpu-fancurve.conf"
echo "-The program from /usr/local/bin/gpu-fancurve"
echo "-The autostart file from $HOME/.config/autostart/gpu-fancurve.desktop"
echo
echo "**sudo rights are required and you will be prompted for your sudo password**"
echo

#Prompt to continue
while true; do
   read -p "Do you want to continue with the uninstall? (YES/no) " prompt
   if [ "${prompt,,}" == "yes" ] || [ "${prompt,,}" == "y" ] || [ "${prompt,,}" == "" ]
   then
      break
   elif [ "${prompt,,}" == "no" ] || [ "${prompt,,}" == "n" ]
   then
      echo "Cancelling uninstall, no changes have been made to the system."
      exit
   fi
   echo "Sorry, I didn't understand that. Please type yes or no"
done

#kill process if running
pkill gpu-fancurve

#remove binary
sudo rm /usr/local/bin/gpu-fancurve

#remove config file
rm $HOME/.config/gpu-fancurve.conf

#remove autostart
rm $HOME/.config/autostart/gpu-fancurve.desktop

