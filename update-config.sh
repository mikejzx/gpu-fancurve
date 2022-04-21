#!/bin/bash
if [ -f "gpu-fancurve.conf" ]
then
   echo "Copying ./gpu-fancurve.conf to ~/.config/gpu-fancurve.conf..."
   cp ./gpu-fancurve.conf ~/.config/
else
   if [ -f "gpu-fancurve.conf.example" ]
   then
      while true; do
         read -p "Couldn't find gpu-fancurve.conf.  Use example configuration? (YES/no) " prompt
         if [ "${prompt,,}" == "yes" ] || [ "${prompt,,}" == "y" ] || [ "${prompt,,}" == "" ]
         then
            echo "Copying gpu-fancurve.conf.example to ~/.config/gpu-fancurve.conf..."
            cp gpu-fancurve.conf.example ~/.config/gpu-fancurve.conf
            break
         elif [ "${prompt,,}" == "no" ] || [ "${prompt,,}" == "n" ]
         then
            echo "Cancelling update, no changes have been made to the system."
            exit
         fi
         echo "Sorry, I didn't understand that. Please type yes or no"
      done
   else
      echo "Couldn't find ./gpu-fancurve.conf or ./gpu-fancurve.conf.example.  No changes have been made to the system."
      exit
   fi
fi
if ! [ -f "~/.config/gpu-fancurve.conf" ]
then
   echo "Couldn't create ~/.config/gpu-fancurve.conf.  Please create the file manually using the example file as a guide."
fi

echo "Killing and restarting gpu-fancurve..."
pkill gpu-fancurve
(gpu-fancurve >/dev/null 2>&1) &

echo "Done!"
