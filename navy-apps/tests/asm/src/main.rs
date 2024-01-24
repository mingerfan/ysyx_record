use std::fs;
use std::process::Command;


fn main() {
    // 用你想遍历的文件夹路径替换
    let path = "../";

    if let Ok(entries) = fs::read_dir(path) {
        for entry in entries {
            if let Ok(entry) = entry {
                let dir_path = entry.path();
                if dir_path.is_dir() {
                    let obj_name = dir_path.file_name().unwrap();
                    let build_path = dir_path.join("build");
                    if build_path.exists() {
                        let output_path = build_path.join("objdump_output.txt");
                        let file_name = format!("{}-riscv64", obj_name.to_str().unwrap());
                        let file = build_path.join(file_name);
                        println!("build_path: {}", file.display());
                        let status = Command::new("riscv64-linux-gnu-objdump")
                            .arg("-d") // 使用 "-d" 参数以反汇编所有代码区
                            .arg(&file)
                            .output()
                            .expect("Failed to execute command");

                        // 将输出写入文件
                        fs::write(output_path, status.stdout).expect("Unable to write file");
                    }
                }
            }
        }
    }
}