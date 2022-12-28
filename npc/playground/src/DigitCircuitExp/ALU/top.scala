package ALU
import chisel3._
import chisel3.util._

class aluAbs(width: Int) extends Module {
    val io = IO(new Bundle {
        val A = Input(UInt(width.W))
        val B = Input(UInt(width.W))
        val out = Output(UInt(width.W))
    })
}

class AndG(width: Int) extends aluAbs(width) {
    io.out := io.A & io.B
}

class OrG(width: Int) extends aluAbs(width) {
    io.out := io.A | io.B
}

class XorG(width: Int) extends aluAbs(width) {
    io.out := io.A ^ io.B
}

class Cmp(width: Int) extends aluAbs(width) {
    when (io.A(width-1) === 0.U && io.B(width-1) === 0.U) {
        io.out := (io.A < io.B).asUInt
    } .elsewhen (io.A(width-1) === 0.U && io.B(width-1) === 1.U) {
        io.out := 0.U
    } .elsewhen (io.A(width-1) === 1.U && io.B(width-1) === 0.U) {
        io.out := 1.U
    } .otherwise {
        io.out := (io.A < io.B).asUInt
    }
}

class IsEqu(width: Int) extends aluAbs(width) {
    io.out := Mux(io.A === io.B, 1.U, 0.U)
}

class AddSub(width: Int) extends aluAbs(width) {
    val exio = IO(new Bundle {
        val mode = Input(UInt(1.W))
        val carry = Output(Bool())
        val zero = Output(Bool())
        val overflow = Output(Bool())
    })
    val carry_u = WireDefault(0.U(1.W))
    
    val B_ = WireDefault(io.B^Fill(width, exio.mode)) + exio.mode

    val res = Wire(UInt((width+1).W))
    res := io.A +& B_
    carry_u := res(width)
    io.out := res(width-1, 0)
    exio.zero := io.out(width-2, 0) === 0.U
    exio.overflow := (io.A(width-1) === B_(width-1)) && (io.A(width-1) =/= io.out(width-1))
    exio.carry := carry_u === 1.U
}
 

class top extends Module {
    val io = IO(new Bundle {
        val sw = Input(UInt(11.W))
        val led = Output(UInt(7.W))
    })

    val sel = WireDefault(io.sw(10, 8))
    val inA = WireDefault(io.sw(3, 0))
    val inB = WireDefault(io.sw(7, 4))

    val res = WireDefault(0.U(4.W))
    val ares = WireDefault(0.U(3.W))

    val andG = Module(new AndG(4))
    val orG = Module(new OrG(4))
    val xorG = Module(new XorG(4))
    val cmp = Module(new Cmp(4))
    val isEqu = Module(new IsEqu(4))
    val addSub = Module(new AddSub(4))

    io.led := Cat(ares, res)

    andG.io.A := inA
    andG.io.B := inB
    
    orG.io.A := inA
    orG.io.B := inB

    xorG.io.A := inA
    xorG.io.B := inB

    cmp.io.A := inA
    cmp.io.B := inB

    isEqu.io.A := inA
    isEqu.io.B := inB

    addSub.io.A := inA
    addSub.io.B := inB
    addSub.exio.mode := 0.U

    ares := 0.U

    switch (sel) {
        // add
        is ("b000".U) {
            addSub.exio.mode := 0.U
            res := addSub.io.out
            ares := "b100".U
        }
        // sub
        is ("b001".U) {
            addSub.exio.mode := 1.U
            res := addSub.io.out
            ares := addSub.exio.overflow.asUInt ## addSub.exio.zero.asUInt ## addSub.exio.carry.asUInt
        }
        // not
        is ("b010".U) {
            res := ~inA
        }
        // and
        is ("b011".U) {
            res := andG.io.out
        }
        // or
        is ("b100".U) {
            res := orG.io.out
        }
        // xor
        is ("b101".U) {
            res := xorG.io.out
        }
        // cmp
        is ("b110".U) {
            res := cmp.io.out
        }
        // isEqu
        is ("b111".U) {
            res := isEqu.io.out
        }
    }
}