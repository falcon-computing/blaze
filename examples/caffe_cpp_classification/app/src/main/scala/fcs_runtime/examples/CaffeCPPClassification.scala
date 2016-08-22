import Array._
import scala.math._
import scala.util.Random
import java.net._
import org.apache.spark.rdd._
import org.apache.spark.SparkContext
import org.apache.spark.SparkConf
import org.apache.spark.blaze._

import org.opencv.core._
import org.opencv.highgui._
//import org.opencv.highgui.Highgui
import org.opencv.imgproc._

import scala.io

class CaffeCPPClassification() 
  extends Accelerator[Array[Float], Array[Float]] {

  val id: String = "VGG-16"

  def getArgNum(): Int = 0

  def getArg(idx: Int): Option[_] = None

	/*
  override def call(in: Array[Double]): Array[Double] = {
    (in, v.data).zipped.map(_ + _)
  }
  */
}

object Transformer { 

	val height_ = 224
	val width_ = 224
	val channel_ = 3

	var input_geometry_ = new Size(width_, height_)

	/*
	def Name2Mat(img_name: String): Mat = {

		val img = Highgui.imread(img_name)
		img
	}

	def Mat2BlazeInput(mat: Mat): Array[Float] = {
	   val height = mat.rows()
	   val width = mat.cols()
	   val chanl = mat.channels()

	   val arr = new Array[Float](height * width * chanl)

	   for (i <- 0 until height) {
		   for (j <- 0 until width) {
			   val rgb = mat.get(i, j)
				for (k <- 0 until chanl) {
					arr(i * width * chanl + j * chanl + k) = rgb(k).toFloat
				}
		   }
	   }

	   arr
   }
	 */

	def Name2BlazeInput(img_name: String): Array[Float] = {

	  System.loadLibrary(Core.NATIVE_LIBRARY_NAME)

		val img = Highgui.imread(img_name)

	   val height = img.rows()
	   val width = img.cols()
	   val channel = img.channels()

	   // resize
	   var sample = new Mat
	   if (channel == 3 && channel_ == 1) {
		Imgproc.cvtColor(img, sample, Imgproc.COLOR_BGR2GRAY)
	   }
	   else if (channel == 4 && channel_ == 1) {
		Imgproc.cvtColor(img, sample, Imgproc.COLOR_BGRA2GRAY)
	   }
	   else if (channel == 4 && channel_ == 3) {
		Imgproc.cvtColor(img, sample, Imgproc.COLOR_BGRA2BGR)
	   }
	   else if (channel == 1 && channel_ == 3) {
		Imgproc.cvtColor(img, sample, Imgproc.COLOR_GRAY2BGR)
	   }
	   else {
		sample = img
	   }

	   var sample_resized = new Mat
	   if (sample.size() != input_geometry_) {
		Imgproc.resize(sample, sample_resized, input_geometry_)
	   }
	   else {
	   sample_resized = sample
	   }

	   val arr = new Array[Float](height_ * width_ * channel_)

	   for (i <- 0 until height_) {
		   for (j <- 0 until width_) {
			   val rgb = sample_resized.get(i, j)
				for (k <- 0 until channel_) {
					arr(i * width_ * channel_ + j * channel_ + k) = rgb(k).toFloat
				}
		   }
	   }

	   arr
	}
}

object PrintRDDElems {

	def PrintTest(arr: Array[Float]) {
		for (i <- 0 until 10) {
			println(arr(i))
		}
	}

	def PrintPredictions(arr: Array[Float]) {
		val label_file = "/curr/xuechao/tools/caffe/data/ilsvrc12/synset_words.txt"
		val feature_size = 1000
		val labels = io.Source.fromFile(label_file).getLines.toList
		val pairs = scala.collection.mutable.Map[Float, Int]()

		for (i <- 0 until feature_size) {
			pairs += (arr(i) -> i)
		}

		val pairs_sorted = scala.collection.immutable.ListMap(pairs.toSeq.sortWith(_._1 > _._1):_*)

		val maxN = pairs_sorted.take(5)
		for ((k, v) <- maxN) {
			val first = labels(v)
			val second = arr(v)
			println(s"$second - $first")
		}
	}
}

object TestApp {
  def main(args : Array[String]) {

	  System.loadLibrary(Core.NATIVE_LIBRARY_NAME)

		  /*
		  val path = System.getProperty("java.library.path")

		  println(path)
		  */

    val conf = new SparkConf()
    conf.setAppName("TestApp")

    val sc = new SparkContext(conf)
    val acc = new BlazeRuntime(sc)

    var num_images = 2
    var num_part = 1
	var feature_size = 1000


	/*
    if (args.size == 2) {
      num_images = args(0).toInt
      num_part = args(1).toInt
    }
	*/

	val img_list = args(0)
//	println(img_list)
	val rdd_img_names = sc.textFile(img_list).repartition(num_part)

//	rdd_img_names.repartition(num_part)

	rdd_img_names.collect().map(println)

	val rdd_img_data = rdd_img_names.map(Transformer.Name2BlazeInput)

	rdd_img_data.collect().map(println)
//	rdd_img_data.collect().map(PrintRDDElems.PrintTest)

//	rdd_img_data.repartition(num_part)

	val start_ts = System.nanoTime()
    val rdd_img_data_acc = acc.wrap(rdd_img_data)
    val res_acc = rdd_img_data_acc.map_acc(new CaffeCPPClassification).collect
	val stop_ts = System.nanoTime()
	println("Elapsed time: " + (stop_ts - start_ts) / 1e6 + "ms")

	res_acc.map(PrintRDDElems.PrintPredictions)

    acc.stop()
  }
}

