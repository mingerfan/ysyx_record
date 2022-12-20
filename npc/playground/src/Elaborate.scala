import chisel3._

object Elaborate extends App {
  def top = new gcd.GCD()
  emitVerilog(top, Array("--target-dir", "./build"))
}
