use std::io::stdin;

mod wlan;
mod ipc;

use wlan::share::frame::User;
use crate::wlan::native::core::connect;
use crate::wlan::share::isp;

fn header(){
    println!();
    println!("Swufe Netool Kernel v1.0");
    println!();
    println!("Using Free Software License v1.0 or later");
    println!("Develop together on https://github.com/xksyu2021/swufe_netool");
    println!("Free software. If you paid for this, please request a refund and report the merchant.");
    println!();
    println!("Dedicated for experts. Ordinary users use the GUI version please.");
    println!();
}

#[tokio::main]
async fn main() {
    header();

    let mut isp_param =  String::new();
    println!("#000");
    println!("Type in the ISP name. It should be one of these following:");
    let mut tmp = false;
    for item in &isp::LIST{
        if tmp {
            print!(" | ")
        }
        tmp = true;
        print!("{}", item.name);
    }
    println!();
    stdin().read_line(&mut isp_param);
    let isp_param = isp_param.trim().to_string();

    let mut account = String::new();
    println!("#001");
    println!("Type in your account (student ID).");
    stdin().read_line(&mut account);
    let account = account.trim().to_string();

    let mut password = String::new();
    println!("#002");
    println!("Type in your password. Echo is closed for safety.");
    stdin().read_line(&mut password);
    let password = password.trim().to_string();

    println!();
    let user = User{
        account,
        password: Default::default()
    };

    for item in isp::LIST {
        if isp_param == item.name {
            println!("#003");
            println!("Connecting...");

            match connect(item, &user).await {
                Ok(true) => {
                    println!("#005");
                    println!("Success! You can exit safely.");
                    return;
                }
                Ok(false) => {
                    eprintln!("#004");
                    eprintln!("FAILED");
                    return;
                }
                Err(_) => {
                    eprintln!("#801");
                    eprintln!("FAILED(Sys|Self): Unknown Ansyc Error!");

                    eprintln!("#004");
                    eprintln!("FAILED");
                    return;
                }
            }
        }
    };

    eprintln!("#800");
    eprintln!("FAILED(Usr): No ISP matchs!");

    eprintln!("#004");
    eprintln!("FAILED");
    return;
}