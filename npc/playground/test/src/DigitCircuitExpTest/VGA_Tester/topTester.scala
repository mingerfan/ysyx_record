package DCE_VGA
import chisel3._
import chiseltest._
import org.scalatest.flatspec.AnyFlatSpec


class topTester extends AnyFlatSpec with ChiselScalatestTester {
    "VGAtop" should "pass" in {
        test(new top) .withAnnotations(Seq(WriteVcdAnnotation)) { dut=>
            def step(n: Int) = {
                for (i <- 0 until n) {
                    dut.clock.step()
                }
            }
            step(800)
        }
    }
}