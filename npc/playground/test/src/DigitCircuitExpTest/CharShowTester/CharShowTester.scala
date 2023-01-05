package CharShow
import chisel3._
import chiseltest._
import org.scalatest.flatspec.AnyFlatSpec

class topTester extends AnyFlatSpec with ChiselScalatestTester {
    "Chartop" should "pass" in {
        test(new top) .withAnnotations(Seq(WriteVcdAnnotation)) { dut=>
            def step(n: Int) = {
                for (i <- 0 until n) {
                    dut.clock.step()
                }
            }
            def ps2Gen(gap: Int, data: Bool) = {
                dut.io.ps2_clk.poke(true.B)
                dut.io.ps2_data.poke(data)
                step(gap)
                dut.io.ps2_clk.poke(false.B)
                step(gap)
            }
            def ps2GenPer(c: Char) = {
                ps2Gen(15, false.B)
                val char_ch = c.U
                var cnt = 0
                for (i <- 0 until 8) {
                    if (char_ch(i) == true.B) {
                        cnt += 1
                    }
                }
                for (i <- 0 until 8) {
                    ps2Gen(15, (char_ch(i)).asBool)
                }
                ps2Gen(15, (cnt%2==0).B)
                ps2Gen(15, true.B)
            }
            def ps2GenPerS(s: String, p: Boolean) = {
                println("current:" + s)
                ps2Gen(15, false.B)
                val char_ch = s.U

                for (i <- 0 until 8) {
                    ps2Gen(15, (char_ch(i)).asBool)
                }
                ps2Gen(15, p.B)
                ps2Gen(15, true.B)
            }
            dut.clock.setTimeout(0)
            dut.io.ps2_clk.poke(false.B)
            dut.io.ps2_data.poke(false.B)

            ps2GenPerS("h32", false)
            ps2GenPerS("hF0", true)
            ps2GenPerS("h32", false)

            ps2GenPerS("h1C", false)
            ps2GenPerS("h12", true)
            step(60000)
        }
    }
}