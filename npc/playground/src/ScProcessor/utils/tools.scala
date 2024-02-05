package ScProcessor
package utils
import chisel3._
import chisel3.util._
import scala.math._
import scala.reflect.ClassTag
import scala.reflect.ClassTag

object tools {

    def MapKeyToArray[T: ClassTag, T1](m: Map[T, T1]) = {
        val arr = new Array[T](m.size)
        var cnt = 0
        for (key <- m.keySet) {
            arr(cnt) = key
            cnt += 1
        }
        arr
    }

    // to get the particular bit of x, hinted by the index of str in array y
    def U_HIT_CURRYING(x: UInt, y: Array[String])(str: String) = {
        // println(s"$str:" + y.indexOf(str))
        x(y.indexOf(str))
    }

}