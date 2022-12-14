package DCE_VGA
import chisel3._
import chisel3.util._

class VGA_Ctrl extends Module {
    val io = IO(new Bundle {
        val pclk = Input(Clock())
        val vgaData = Input(UInt(24.W))
        val hAddr = Output(UInt(10.W))
        val vAddr = Output(UInt(10.W))
        val hsync = Output(Bool())
        val vsync = Output(Bool())
        val valid = Output(Bool())
        val vgaR = Output(UInt(8.W))
        val vgaG = Output(UInt(8.W))
        val vgaB = Output(UInt(8.W))
    })
    val h_frontporch = 96
    val h_active = 144
    val h_backporch = 784
    val h_total = 800

    val v_frontporch = 2
    val v_active = 35
    val v_backporch = 515
    val v_total = 525

    withClock (io.pclk) {
        val xCnt = RegInit(1.U(log2Ceil(h_total).W))
        val yCnt = RegInit(1.U(log2Ceil(v_total).W))

        xCnt := xCnt + 1.U
        when (xCnt >= h_total.U) {
            xCnt := 1.U
        }

        when (xCnt === h_total.U && yCnt >= v_total.U) {
            yCnt := 1.U
        } .elsewhen (xCnt === h_total.U) {
            yCnt := yCnt + 1.U
        }

        val h_valid = WireDefault((xCnt > h_active.U) && (xCnt <= h_backporch.U))
        val v_valid = WireDefault((yCnt > v_active.U) && (yCnt <= v_backporch.U))

        io.hsync := xCnt > h_frontporch.U
        io.vsync := yCnt > v_frontporch.U
        io.valid := h_valid && v_valid
        io.hAddr := Mux(h_valid, xCnt - h_active.U - 1.U, 0.U)
        io.vAddr := Mux(v_valid, yCnt - v_active.U - 1.U, 0.U)

        io.vgaR := io.vgaData(23,16)
        io.vgaG := io.vgaData(15,8)
        io.vgaB := io.vgaData(7,0)
    }
}