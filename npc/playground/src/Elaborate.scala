import chisel3._
import chisel3.util._

object Elaborate extends App {
    def top = new HomeWorkHDL.top
    emitVerilog(top, Array("--target-dir", "./build"))
}