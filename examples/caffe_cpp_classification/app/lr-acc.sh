#!/bin/bash
source config.sh
export HADOOP_CONF_DIR

$SPARK_HOME/bin/spark-submit --class LogisticRegression \
	--driver-memory 8G \
	--executor-memory 8G \
	--executor-cores 4 \
	--num-executors 2 \
  --conf spark.app.name="Logistic-ACC" \
  --conf spark.yarn.executor.nodeLabelExpression="LogisticGradientAndLoss" \
	--master yarn-cluster \
	logistic-1.0.jar $DATA $NUM_PARTITION &> lr-acc.${HOSTNAME}.log &

