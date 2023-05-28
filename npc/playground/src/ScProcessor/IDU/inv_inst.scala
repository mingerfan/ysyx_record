package ScProcessor
import chisel3._
import chisel3.util._
import topInfo._

class inv_inst extends BlackBox {
    val io = IO(new Bundle {
        val inv = Input(Bool())
        val clk = Input(Bool())
        val pc = Input(UInt(XLEN.W))
    })
}