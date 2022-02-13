#!/bin/bash

FILEPATH=$1

filename=$(basename -- "$FILEPATH")
filenameh="${filename}.h"

arrayName="${filename//./_}"

exportPath="Assets/Compiled/CompiledResources"
exportFile="${exportPath}/${filenameh}"

rm -f $exportFile
echo "Exporting $FILEPATH to $exportFile"

echo "#pragma once" 					   > $exportFile
echo "" 								  >> $exportFile
echo "namespace LegendEngine::Resources " >> $exportFile
echo "{" 								  >> $exportFile
bin2c --name $arrayName $FILEPATH 		  >> $exportFile
echo "}" 								  >> $exportFile