use std::fmt::{self};
use std::fs::File;
use std::io::Write;

enum Para {
    FixInt(f64, i32),
    FixFix(f64, f64),
    Fix(f64),
}


use rand::{rngs::ThreadRng, Rng};
use Para::*;

impl Para {
    fn to_string_(&self) -> String {
        match self {
            FixInt(f, i) => 
                format!("fixedpt_rconst({}), {}", f, i).to_string(),
            FixFix(f, f1) =>
                format!("fixedpt_rconst({}), fixedpt_rconst({})", f, f1).to_string(),
            Fix(f) =>
                format!("fixedpt_rconst({})", f).to_string(),
        }
    }

    fn refill(&mut self, r: &mut ThreadRng) {
        match self {
            FixInt(f, i) => {
                *f = r.gen_range(-1000.0..1000.0);
                *i = r.gen_range(-1000..1000);
            },
            FixFix(f, f1) => {
                *f = r.gen_range(-1000.0..1000.0);
                *f1 = r.gen_range(-1000.0..1000.0);
            },
            Fix(f) => {
                *f = r.gen_range(-1000.0..1000.0);
            }
        }
    }
}

impl fmt::Display for Para {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.to_string_())
    }
}

struct Fun
{
    f_name: String,
    f_para: Para,
    op: fn(&Para) -> Para,
    err_rate: f64,
}

impl Fun {
    fn new(f_name_: &str, f_para: Para, op: fn(&Para) -> Para, err_rate: f64) -> Fun {
        Fun {
            f_name: f_name_.to_string(),
            f_para,
            op,
            err_rate,
        }
    }

    fn gen_string(&self) -> String {
        let return_para = (self.op)(&self.f_para);
        let res = match return_para {
            Fix(x) => x,
            _ => panic!("Should not reach here!"),
        };
        let err = ((self.err_rate * res * 256f64) as i32).abs();
        if err == 0 && self.err_rate != 0f64 {
            return format!("printf(\"{0}: %d %d\\n\", {0}({1}), {2});\n\tassert(\
                abs({0}({1}) - {2}) <= {3}\
            );", 
            self.f_name, 
            self.f_para, 
            return_para,
            1, )
        }
        format!("printf(\"{0}: %d %d\\n\", {0}({1}), {2});\n\tassert(\
            abs({0}({1}) - {2}) <= {3}\
        );", 
        self.f_name, 
        self.f_para, 
        return_para,
        err, )
    }
}

fn muli(a: &Para) -> Para {
    if let FixInt(f, i) = a {
        Fix(f * (*i) as f64)
    } else {
        panic!("Should not reach here!")
    }
}

fn divi(a: &Para) -> Para {
    if let FixInt(f, i) = a {
        Fix(f / (*i) as f64)
    } else {
        panic!("Should not reach here!")
    }
}

fn mul(a: &Para) -> Para {
    if let FixFix(f, f1) = a {
        Fix(f * f1)
    } else {
        panic!("Should not reach here!")
    }
}

fn div(a: &Para) -> Para {
    if let FixFix(f, f1) = a {
        Fix(f / f1)
    } else {
        panic!("Should not reach here!")
    }
}

fn abs(a: &Para) -> Para {
    if let Fix(f) = a {
        let f_round = (f * 256f64).round() / 256f64;
        Fix(if f_round > 0f64 { f_round } else { -f_round })
    } else {
        panic!("Should not reach here!")
    }
}

fn floor(a: &Para) -> Para {
    if let Fix(f) = a {
        let f_round = (f * 256f64).round() / 256f64;
        Fix(f_round.floor())
    } else {
        panic!("Should not reach here!")
    }
}

fn ceil(a: &Para) -> Para {
    if let Fix(f) = a {
        let f_round = (f * 256f64).round() / 256f64;
        Fix(f_round.ceil())
    } else {
        panic!("Should not reach here!")
    }
}

fn main() -> std::io::Result<()>{
    let main_path = "../main.c";
    let mut file = File::create(main_path)?;

    writeln!(&mut file, 
    "#include <stdio.h>\n\
    #include <fixedptc.h>\n\
    #include <assert.h>\n\
    #include <stdlib.h>\n\
    \n\
    int main() {{")?;

    let mut rng = rand::thread_rng();

    let mut fun_vec = vec![
        Fun::new("fixedpt_muli", FixInt(0.0, 0), muli, 0.08),
        Fun::new("fixedpt_divi", FixInt(0.0, 0), divi, 0.08),
        Fun::new("fixedpt_mul", FixFix(0.0, 0.0), mul, 0.08),
        Fun::new("fixedpt_div", FixFix(0.0, 0.0), div, 0.08),
        Fun::new("fixedpt_abs", Fix(0.0), abs, 0.0),
        Fun::new("fixedpt_floor", Fix(0.0), floor, 0.0),
        Fun::new("fixedpt_ceil", Fix(0.0), ceil, 0.0),
    ];

    for _ in 1..300 {
        let idx = rng.gen_range(0..=6);
        fun_vec[idx].f_para.refill(&mut rng);
        if fun_vec[idx].err_rate == 0f64 {
            writeln!(file, "\tprintf(\"###{}: %d### following>>>\\n\", {});", fun_vec[idx].f_name, fun_vec[idx].f_para)?;
        }
        writeln!(file, "\t{}", fun_vec[idx].gen_string())?;
    }

    writeln!(file, "\treturn 0;\n}}")?;

    Ok(())
}
