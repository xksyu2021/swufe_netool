mod wlan;

use std::io::stdin;
use wlan::share::frame::User;
use crate::wlan::native::core::connect;
use crate::wlan::share::isp;
use rpassword::read_password;
use secrecy::SecretString;

#[tokio::main]
async fn main() {
    // input info
    let mut isp_param =  String::new();
    println!("*isp#");
    stdin().read_line(&mut isp_param);
    let isp_param = isp_param.trim().to_string();

    let mut account = String::new();
    println!("*acc#");
    stdin().read_line(&mut account);
    let account = account.trim().to_string();

    println!("*pwd#");
    let raw = match read_password() {
        Ok(pwd) => pwd,
        Err(e) => {
            println!("*102 {e}#");
            return;
        }
    };
    let password = SecretString::new(Box::from(raw));

    println!();
    let user = User{
        account,
        password
    };

    // call connect function
    let mut wait = String::new();
    for item in isp::LIST {
        if isp_param == item.name {
            match connect(item, &user).await {
                Ok(true) => {
                    println!("*suc#");
                    stdin().read_line(&mut wait);
                    return;
                }
                Ok(false) => {
                    stdin().read_line(&mut wait);
                    return;
                }
                Err(e) => {
                    eprintln!("*100 {e}#");
                    stdin().read_line(&mut wait);
                    return;
                }
            }
        }
    };
    eprintln!("*102 No ISP matched.#");
    stdin().read_line(&mut wait);
    return;
}