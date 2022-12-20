package gcd
import chisel3._

object Elaborate extends App {
  def top = new GCD()
  emitVerilog(new top, Array("--target-dir", "./build"))
}
