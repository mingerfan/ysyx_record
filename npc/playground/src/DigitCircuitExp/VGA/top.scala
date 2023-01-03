package DCE_VGA
import chisel3._
import chisel3.util._
import chisel3.util.experimental.loadMemoryFromFileInline

class top extends Module {
    val io = IO(new Bundle {
        val vgaVsync = Output(Bool())
        val vgaHsync = Output(Bool())
        val vgaBlank = Output(Bool())
        val vgaR = Output(UInt(8.W))
        val vgaG = Output(UInt(8.W))
        val vgaB = Output(UInt(8.W))
    })

    val vgaCtrl = Module(new VGA_Ctrl)
    val mem = Module(new MemRom(24, 19, "/home/xs/ysyx/ysyx-workbench/nvboard/picture.hex"))
    mem.io.addr := Cat(vgaCtrl.io.hAddr, vgaCtrl.io.vAddr)
    vgaCtrl.io.pclk := clock
    vgaCtrl.io.vgaData := io.addr
    io.vgaHsync := vgaCtrl.io.hsync
    io.vgaVsync := vgaCtrl.io.vsync
    io.vgaBlank := vgaCtrl.io.valid
    io.vgaR := vgaCtrl.io.vgaR
    io.vgaG := vgaCtrl.io.vgaG
    io.vgaB := vgaCtrl.io.vgaB
}

class MemRom(addr_width: Int, out_width: Int, init_file: String) extends AbsRom(addr_width, out_width) {
    val mem = SyncReadMem(max_num, UInt(out_width.W))
    loadMemoryFromFileInline(mem, init_file)
    // This prevents deduping this memory module
    // Ref. https://github.com/chipsalliance/firrtl/issues/2168
    val dedupBlock = WireInit(mem.hashCode.U)

    io.out := mem.read(io.addr)
}
