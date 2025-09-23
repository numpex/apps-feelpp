#!/bin/sh
#SBATCH -p public
#SBATCH -n 10
#SBATCH -J test
#SBATCH -o job%j-o.log
#SBATCH -e job%j-e.log
echo "Hello world"
mpirun -np 10 -bind-to core feelpp_toolbox_heat --config-file /usr/share/feelpp/data/testcases/toolboxes/heat/cases/Building/ThermalBridgesENISO10211/case3.cfg