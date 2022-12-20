package gcd
import chisel3._

object Elaborate extends App {
  def top = new GCD()
  emitVerilog(new Hello(), Array("--target-dir", "./build"))
}
