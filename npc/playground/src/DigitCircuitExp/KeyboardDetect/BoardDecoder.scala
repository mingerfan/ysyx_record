package KeyboardDetect
import chisel3._
import chisel3.util._

class CtrlDec extends Module {
    val io = IO(new Bundle {
        val In = Input(UInt(16.W))
        val isCtrl = Output(Bool())
        val enter = Output(Bool())
        val shift = Output(Bool())
        val backspace = Output(Bool())
        val space = Output(Bool())
    })

    io.isCtrl := io.enter || io.shift || io.backspace || io.space

    io.enter := false.B
    io.shift := false.B
    io.backspace := false.B
    io.space := false.B

    switch (io.In) {
        is ("h5A".U) { io.enter := true.B }
        is ("h12".U) { io.shift := true.B }
        is ("h59".U) { io.shift := true.B }
        is ("h66".U) { io.backspace := true.B }
        is ("h29".U) { io.space := true.B }
    }
}

class CharDec extends Module {
    val io = IO(new Bundle {
        val In = Input(UInt(16.W))
        val num = Output(Bool())
        val letterD = Output(Bool())
        val others = Output(Bool())
        val dec_step1 = Output(UInt(8.W))
    })

    io.num := false.B
    io.letterD := false.B
    io.others := false.B
    io.dec_step1 :=  0.U

    switch (io.In) {
        // NUM
        is ("h16".U) {
            io.num := true.B
            io.dec_step1 := '1'.U
        }
        is ("h1E".U) {
            io.num := true.B
            io.dec_step1 := '2'.U
        }
        is ("h26".U) {
            io.num := true.B
            io.dec_step1 := '3'.U
        }
        is ("h25".U) {
            io.num := true.B
            io.dec_step1 := '4'.U
        }
        is ("h2E".U) {
            io.num := true.B
            io.dec_step1 := '5'.U
        }
        is ("h36".U) {
            io.num := true.B
            io.dec_step1 := '6'.U
        }
        is ("h3D".U) {
            io.num := true.B
            io.dec_step1 := '7'.U
        }
        is ("h3E".U) {
            io.num := true.B
            io.dec_step1 := '8'.U
        }
        is ("h46".U) {
            io.num := true.B
            io.dec_step1 := '9'.U
        }
        is ("h45".U) {
            io.num := true.B
            io.dec_step1 := '0'.U
        }
        // LETTER DOWN
        is ("h1C".U) {
            io.letterD := true.B
            io.dec_step1 := 'a'.U
        }
        is ("h32".U) {
            io.letterD := true.B
            io.dec_step1 := 'b'.U
        }
        is ("h21".U) {
            io.letterD := true.B
            io.dec_step1 := 'c'.U
        }
        is ("h23".U) {
            io.letterD := true.B
            io.dec_step1 := 'd'.U
        }
        is ("h24".U) {
            io.letterD := true.B
            io.dec_step1 := 'e'.U
        }
        is ("h2B".U) {
            io.letterD := true.B
            io.dec_step1 := 'f'.U
        }
        is ("h34".U) {
            io.letterD := true.B
            io.dec_step1 := 'g'.U
        }
        is ("h33".U) {
            io.letterD := true.B
            io.dec_step1 := 'h'.U
        }
        is ("h43".U) {
            io.letterD := true.B
            io.dec_step1 := 'i'.U
        }
        is ("h3B".U) {
            io.letterD := true.B
            io.dec_step1 := 'j'.U
        }
        is ("h42".U) {
            io.letterD := true.B
            io.dec_step1 := 'k'.U
        }
        is ("h4B".U) {
            io.letterD := true.B
            io.dec_step1 := 'l'.U
        }
        is ("h3A".U) {
            io.letterD := true.B
            io.dec_step1 := 'm'.U
        }
        is ("h31".U) {
            io.letterD := true.B
            io.dec_step1 := 'n'.U
        }
        is ("h44".U) {
            io.letterD := true.B
            io.dec_step1 := 'o'.U
        }
        is ("h4D".U) {
            io.letterD := true.B
            io.dec_step1 := 'p'.U
        }
        is ("h15".U) {
            io.letterD := true.B
            io.dec_step1 := 'q'.U
        }
        is ("h2D".U) {
            io.letterD := true.B
            io.dec_step1 := 'r'.U
        }
        is ("h1B".U) {
            io.letterD := true.B
            io.dec_step1 := 's'.U
        }
        is ("h2C".U) {
            io.letterD := true.B
            io.dec_step1 := 't'.U
        }
        is ("h3C".U) {
            io.letterD := true.B
            io.dec_step1 := 'u'.U
        }
        is ("h2A".U) {
            io.letterD := true.B
            io.dec_step1 := 'v'.U
        }
        is ("h1D".U) {
            io.letterD := true.B
            io.dec_step1 := 'w'.U
        }
        is ("h22".U) {
            io.letterD := true.B
            io.dec_step1 := 'x'.U
        }
        is ("h35".U) {
            io.letterD := true.B
            io.dec_step1 := 'y'.U
        }
        is ("h1A".U) {
            io.letterD := true.B
            io.dec_step1 := 'z'.U
        }
        // OTHERS
        is ("h0E".U) {
            io.others := true.B
            io.dec_step1 := '`'.U
        }
        is ("h4E".U) {
            io.others := true.B
            io.dec_step1 := '-'.U
        }
        is ("h55".U) {
            io.others := true.B
            io.dec_step1 := '='.U
        }
        is ("h5D".U) {
            io.others := true.B
            io.dec_step1 := '\\'.U
        }
        is ("h54".U) {
            io.others := true.B
            io.dec_step1 := '['.U
        }
        is ("h5B".U) {
            io.others := true.B
            io.dec_step1 := ']'.U
        }
        is ("h4C".U) {
            io.others := true.B
            io.dec_step1 := '\''.U
        }
        is ("h41".U) {
            io.others := true.B
            io.dec_step1 := ','.U
        }
        is ("h49".U) {
            io.others := true.B
            io.dec_step1 := '.'.U
        }
        is ("h4A".U) {
            io.others := true.B
            io.dec_step1 := '/'.U
        }
    }
}

class FinalDec extends Module {
    val io = IO(new Bundle {
        val shift = Input(Bool())
        val space = Input(Bool())
        val enter = Input(Bool())
        val In = Input(UInt(16.W))
        val dec = Output(UInt(8.W))
    })
    io.dec := io.In

    when (io.shift) {
        when (io.In >= 'a'.U && io.In <= 'z'.U) {
            io.dec := io.In - 32.U
        } .otherwise {
            switch (io.In) {
                is ('1'.U) {
                    io.dec := '!'.U
                }
                is ('2'.U) {
                    io.dec := '@'.U
                }
                is ('3'.U) {
                    io.dec := '#'.U
                }
                is ('4'.U) {
                    io.dec := '$'.U
                }
                is ('5'.U) {
                    io.dec := '%'.U
                }
                is ('6'.U) {
                    io.dec := '^'.U
                }
                is ('7'.U) {
                    io.dec := '&'.U
                }
                is ('8'.U) {
                    io.dec := '*'.U
                }
                is ('9'.U) {
                    io.dec := '('.U
                }
                is ('0'.U) {
                    io.dec := ')'.U
                }
                is ('`'.U) {
                    io.dec := '~'.U
                }
                is ('-'.U) {
                    io.dec := '_'.U
                }
                is ('='.U) {
                    io.dec := '+'.U
                }
                is ('['.U) {
                    io.dec := '{'.U
                }
                is (']'.U) {
                    io.dec := '}'.U
                }
                is ('\\'.U) {
                    io.dec := '|'.U
                }
                is (';'.U) {
                    io.dec := ':'.U
                }
                is ('\''.U) {
                    io.dec := '"'.U
                }
                is (','.U) {
                    io.dec := '<'.U
                }
                is ('.'.U) {
                    io.dec := '>'.U
                }
                is ('/'.U) {
                    io.dec := '?'.U
                }
            }
        }
    } .elsewhen (io.enter) {
        io.dec := '\n'.U
    } .elsewhen (io.space) {
        io.dec := ' '.U
    }
}