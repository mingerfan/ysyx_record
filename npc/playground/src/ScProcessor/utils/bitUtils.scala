package ScProcessor
package utils
import chisel3._
import chisel3.util._
import scala.math._

object bitUtils {
    def ZeroExt(x: UInt, len: Int) = {
        val xlen = x.getWidth
        require(xlen < len)
        Cat(0.U((len - xlen).W), x)
    }

    def SignExt(x: UInt, len: Int) = {
        val xlen = x.getWidth
        val sign_bit = x(xlen - 1)
        require(xlen < len)
        Cat(Fill(len - xlen, sign_bit), x)
    }

    // extend a UInt to 64bits
    def U_SEXT64(x: UInt, len: Int) = {
        require(len >= 0);
        SignExt(x(len - 1, 0), 64)
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
        (Fill(high + 1, 1.U(1.W)) >> low  << low).asUInt
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
