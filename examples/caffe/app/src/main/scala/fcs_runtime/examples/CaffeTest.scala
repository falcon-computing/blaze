import Array._
import scala.math._
import scala.util.Random
import java.net._
import org.apache.spark.rdd._
import org.apache.spark.SparkContext
import org.apache.spark.SparkConf
import org.apache.spark.blaze._

//class CaffeTest(v: BlazeBroadcast[Array[Double]]) 
//  extends Accelerator[Array[Double], Array[Double]] {
class CaffeTest() 
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

object TestApp {
  def main(args : Array[String]) {

    val conf = new SparkConf()
    conf.setAppName("TestApp")

    val sc = new SparkContext(conf)
    val acc = new BlazeRuntime(sc)

	var im_height = 224
	var im_width = 224
	var im_size = im_height * im_width * 3
    var num_images = 16
    var num_part = 2

    if (args.size == 2) {
      num_images = args(0).toInt
      num_part = args(1).toInt
    }

    val data = Array.fill(num_images)(Array.fill(im_size)(Random.nextFloat));
	/*
	val data = Array.ofDim[Float](1, 10)

		for (i <- 0 until 10) {
			data(0)(i) = i
		}
		*/

    val rdd = sc.parallelize(data, num_part)
    val rdd_acc = acc.wrap(rdd)
    
    val res_acc = rdd_acc.map_acc(new CaffeTest).collect

    acc.stop()
  }
}

