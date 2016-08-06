#!/bin/bash

#if [ -z $FCS_RT_ROOT ]; then
#  FCS_RT_ROOT=../../..
#fi
#if [ ! -f "$FCS_RT_ROOT/nam/bin/nam_daemon" ]; then
#  echo "FCS_RT_ROOT seems incorrect, please set it to the root directory of FCS_RUNTIME"
#  exit -1
#fi
FCS_RT_ROOT=/curr/diwu/prog/fcs_runtime
SPARK_HOME=$FCS_RT_ROOT/spark-1.5.1-bin-fcs
$SPARK_HOME/bin/spark-submit --class TestApp \
		--driver-library-path /curr/diwu/tools/opencv-2.4.12/share/OpenCV/java \
	--jars /curr/diwu/tools/opencv-2.4.12/share/OpenCV/java/opencv-2412.jar \
	--conf "spark.driver.extraLibraryPath=/curr/diwu/tools/opencv-2.4.12/share/OpenCV/java:/curr/diwu/tools/opencv-2.4.12/build/lib" \
	--conf "spark.executor.extraLibraryPath=/curr/diwu/tools/opencv-2.4.12/share/OpenCV/java:/curr/diwu/tools/opencv-2.4.12/build/lib" \
	--driver-memory 2G \
	--executor-memory 1G \
	--master local[*] \
	target/caffecppclassification-1.0.jar $@

