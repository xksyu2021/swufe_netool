mod wlan;

use std::io::stdin;
use wlan::share::frame::User;
use crate::wlan::native::core::connect;
use crate::wlan::share::isp;
use rpassword::read_password;
use secrecy::SecretString;

#[tokio::main]
async fn main() {
    let mut isp_param =  String::new();
    println!("*000");
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

    println!("#002");
    println!("Type in your password. Echo is closed for safety.");
    let raw = match read_password() {
        Ok(pwd) => pwd,
        Err(_) => {
            println!("#802");
            println!("FAILED(Sys): IO error!");
            return;
        }
    };
    let password = SecretString::new(Box::from(raw));

    println!();
    let user = User{
        account,
        password
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