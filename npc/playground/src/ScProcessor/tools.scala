package ScProcessor
import chisel3._
import chisel3.util._
import scala.math._

object tools {
    // extend a UInt to 64bits
    def U_SEXT64(x: UInt, len: Int) = {
        (x(len-1, 0))|(Fill(64-len, x(len-1))<<len.U)
    }

    // to get the particular bit of x, hinted by the index of str in array y
    def U_HIT_CURRYING(x: UInt, y: Array[String])(str: String) = {
        // println(s"$str:" + y.indexOf(str))
        x(y.indexOf(str))
    }

    // set bits to zero, which between low and high bit are reserved
    // low and high arrange from 0 to 63
    def MASK(x: BigInt, high: Int, low: Int) = {
        require(low >= 0)
        require(high < 64)
        require(low <= high)
        ~((1<<low)-1) & x & ((1<<(high+1))-1)
    }

    // select particular bits, like x[high:low] in verilog
    // 0 to 63
    def BITS(x: BigInt, high: Int, low: Int) = {
        MASK(MASK(x, high, low)>>low, high-low, 0)
    }

}