#!/bin/bash

command="echo command"

case $1 in
   autodetect)
      # set -e = if any step fails, the script exits with value 1
      set -e
      echo autodetect
      #test -x /etc/init.d/bluetooth
      ## lshal | grep "linux.subsystem" | grep -q "bluetooth" # Do not enable this: so we want to have always bluetooth listening, so we want to always run it even if we dont have a bluetooth plugged

      # exit 1 # autodetection failed
      exit 0
      ;;
   run)
      if ! ps ux | grep -v grep | grep "${command%% *}" 2>/dev/null ; then
         # important: do not use " for run the command and run in bg mode
         ( $command & ) &
      fi

      exit 0
      ;;
   terminate|stop|stopper)
      killall "${command%% *}" 2>/dev/null 1>/dev/null
      killall -9 "${command%% *}" 2>/dev/null 1>/dev/null

      exit 0
      ;;
esac

