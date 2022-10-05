# Setup script for tutorial "Introduction to RooFit" - University of Trieste, October 2022 - Dr. Michele Pinamonti
# Run the script every time you open a new terminal, just with:
#   source setup.sh

source /cvmfs/sft.cern.ch/lcg/app/releases/ROOT/6.20.04/x86_64-centos7-gcc48-opt/bin/thisroot.sh 

# This line is setting an alias to conveniently run a ROOT macro.
# To run a ROOT macro saved in a .C file, juts use the command:
#   macro <file.C>

alias macro="root -l -b -q"
