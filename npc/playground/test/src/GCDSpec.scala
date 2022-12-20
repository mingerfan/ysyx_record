package gcd
import chisel3._
import chiseltest._
import chisel3.experimental.BundleLiterals._

import org.scalatest.flatspec.AnyFlatSpec

/**
  * This is a trivial example of how to run this Specification
  * From within sbt use:
  * {{{
  * testOnly gcd.GcdDecoupledTester
  * }}}
  * From a terminal shell use:
  * {{{
  * sbt 'testOnly gcd.GcdDecoupledTester'
  * }}}
  */
class GCDSpec extends AnyFlatSpec with ChiselScalatestTester{
  "GCD" should "pass" in {
    test(new GCD) .withAnnotations(Seq(WriteVcdAnnotation)) { dut =>
      dut.io.value1.poke(35.U)
      dut.io.value2.poke(75.U)
      dut.io.loadingValues.poke(true.B)
      dut.clock.step()
      dut.io.loadingValues.poke(false.B)
      for (i <- 0 until 100) {
        dut.clock.step()
      }
    }
  }
}
