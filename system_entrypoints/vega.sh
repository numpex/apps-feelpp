#!/bin/bash


#TODO: REMOVE LD_LIBRARY_PATH EXPORTS

module load Python/3.10.8-GCCcore-12.2.0
export LD_LIBRARY_PATH=/cvmfs/sling.si/modules/el7/software/Python/3.10.8-GCCcore-12.2.0/lib:$LD_LIBRARY_PATH
python3 -m venv .venv
echo 'export LD_LIBRARY_PATH=/cvmfs/sling.si/modules/el7/software/Python/3.10.8-GCCcore-12.2.0/lib:$LD_LIBRARY_PATH' >> .venv/bin/activate
source .venv/bin/activate
.venv/bin/python3 -m pip install feelpp-benchmarking