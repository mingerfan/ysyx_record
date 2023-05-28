package ScProcessor
import scala.math._
import scala.reflect.ClassTag
import scala.reflect.ClassTag

object myutil {
    def MapKeyToArray[T: ClassTag, T1](m: Map[T, T1]) = {
        val arr = new Array[T](m.size)
        var cnt = 0
        for (key <- m.keySet) {
            arr(cnt) = key
            cnt += 1
        }
        arr
    }
}