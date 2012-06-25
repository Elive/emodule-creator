#!/bin/bash

command="echo command"

case $1 in
   conf|configure)
      # Run the configuration tool, when we enter in teh configuration panel
      if ! ps ux | grep -v grep | grep -v $0 | grep -q "${command%% *}" 2>/dev/null ; then
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

