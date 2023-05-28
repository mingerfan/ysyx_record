package ScProcessor
import chisel3._
import chisel3.util._
import topInfo._

class ebreak_detect extends BlackBox {
    val io = IO(new Bundle {
        val ebreak = Input(UInt(1.W))
        val clk = Input(Bool())
        val a0 = Input(UInt(XLEN.W))
        val pc = Input(UInt(XLEN.W))
    })
}