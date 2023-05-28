import chisel3._
import chisel3.util._

object Elaborate extends App {
    def top = new ScProcessor.top
    emitVerilog(top, Array("--target-dir", "./build"))
}