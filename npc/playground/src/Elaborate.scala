import chisel3._
import chisel3.util._

object Elaborate extends App {
    def top = new DCE_Mux.top
    emitVerilog(top, Array("--target-dir", "./build"))
}