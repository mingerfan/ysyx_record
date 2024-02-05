package ScProcessor
package utils
import chisel3._
import chisel3.util._
import scala.math._

object bitUtils {
    // extend a UInt to 64bits
    def U_SEXT64(x: UInt, len: Int) = {
        Fill(64-len, x(len-1)) ## (x(len-1, 0))
    }

    // set bits to zero, which between low and high bit are reserved
    // low and high arrange from 0 to 63
    def MASK(x: BigInt, high: Int, low: Int) = {
        require(low >= 0)
        require(high < 64)
        require(low <= high)
        ~((1<<low)-1) & x & ((1<<(high+1))-1)
    }


    def GenMask(high: Int, low: Int) = {
        require(low >= 0);
        require(low < high);
        (VecInit(List.fill(high+1)(true.B)).asUInt >> low  << low).asUInt
    }

    def GenMask(pos: Int) = {
        require(pos >= 0);
        1.U << pos
    }

    // select particular bits, like x[high:low] in verilog
    // 0 to 63
    def BITS(x: BigInt, high: Int, low: Int) = {
        MASK(MASK(x, high, low)>>low, high-low, 0)
    }
}
