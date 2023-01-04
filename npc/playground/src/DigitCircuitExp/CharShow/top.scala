package CharShow
import chisel3._
import chisel3.util._
import KeyboardDetect._
import DCE_VGA._

class top extends Module {
    val io = IO(new Bundle {
        val ps2_clk = Input(Bool())
        val ps2_data = Input(Bool())
        val segs = Output(Vec(6, UInt(7.W)))
        val vgaVsync = Output(Bool())
        val vgaHsync = Output(Bool())
        val vgaBlank = Output(Bool())
        val vgaR = Output(UInt(8.W))
        val vgaG = Output(UInt(8.W))
        val vgaB = Output(UInt(8.W))
    })

    val view = Module(new View)
    val controller = Module(new Controller)
    val vga = Module(new VGA_Ctrl)
    val keyB = Module(new KeyBoard)
    val risingEdgeU = WireDefault((!RegNext(keyB.io.clkout) && keyB.io.clkout).asUInt)

    view.io.hAddr := vga.io.hAddr
    view.io.vAddr := vga.io.vAddr
    view.io.cursorIndex := controller.io.cursorIndex
    view.io.charData := controller.io.charData

    controller.io.backspace := keyB.io.backspace
    controller.io.KeyBoardIn := keyB.io.ascii & Fill(8, risingEdgeU)
    controller.io.charIndex := view.io.charIndex

    vga.io.pclk := clock
    vga.io.vgaData := view.io.vgaData

    keyB.io.ps2_clk := io.ps2_clk
    keyB.io.ps2_data := io.ps2_data
    
    io.segs := keyB.io.segs
    io.vgaVsync := vga.io.vsync
    io.vgaHsync := vga.io.hsync
    io.vgaBlank := vga.io.valid
    io.vgaR := vga.io.vgaR
    io.vgaG := vga.io.vgaG
    io.vgaB := vga.io.vgaB
}