pub mod frame{
    use secrecy::SecretString;

    pub enum AuthType{
        OpenAndWeb = 0,
        Edu,
        None
    }

    pub struct User{
        pub account: String,
        pub password: SecretString
    }

    pub struct Isp{
        pub auth_type: AuthType,
        pub name: String,
        pub ssid: String,
        pub domain: String,
        pub auth_add: String,
        pub auth_port: String
    }

    impl Isp {
        pub fn new(name: String,
                   auth_type: AuthType,
                   ssid: &str,
                   domain: &str,
                   auth_add: &str,
                   auth_port: &str) -> Isp{
            Isp{
                name,
                auth_type,
                ssid: String::from(ssid),
                domain: String::from(domain),
                auth_add: String::from(auth_add),
                auth_port: String::from(auth_port)
            }
        }
    }
}

pub mod isp{
    use std::sync::LazyLock;
    use super::frame::{AuthType, Isp};

    pub static CMCC: LazyLock<Isp> = LazyLock::new(|| {
        Isp::new("cmcc".to_string(),AuthType::OpenAndWeb,"SWUFE_CMCC", "yd", "10.0.7.8", "8088")
    });

    pub static TEST: LazyLock<Isp> = LazyLock::new(|| {
        Isp::new("test".to_string(),AuthType::OpenAndWeb,"nullptr", "org", "xksyu.cn", "443")
    });
    
    pub static LIST: [&LazyLock<Isp>; 2] = [&CMCC, &TEST];
}