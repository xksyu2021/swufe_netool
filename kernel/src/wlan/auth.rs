mod info{
    use serde::{Deserialize, Serialize};

    pub static CL_ADDRESS: &str = "cgi-bin/get_challenge";
    pub static AUTH_ADDRESS: &str = "cgi-bin/srun_portal";
    pub static IP_TEST: &str = "10.9.0.4";

    #[derive(Serialize)]
    pub struct ClParam{
        pub username: String,
        pub ip: String,
        pub double_stack: String,
        pub timestamp: String
    }

    #[derive(Deserialize)]
    pub struct ClResponse{
        pub challenge: Option<String>
    }

    #[derive(Serialize)]
    pub struct PostParam {
        pub action: String,
        pub username: String,
        pub password: String,
        pub ac_id: String,
        pub ip: String,
        pub double_stack: String,
        pub n: String,
        #[serde(rename = "type")]
        pub r#type: String,
        pub os: String,
        pub name: String,
        pub info: String,
        pub chksum: String,
    }
}

pub mod core{
    use super::info;
    use crate::wlan::share::frame::{Isp, User};
    use std::net::UdpSocket;
    use std::time::{SystemTime, SystemTimeError, UNIX_EPOCH};
    use reqwest;

    fn timestamp() -> Result<u128,SystemTimeError> {
        Ok(SystemTime::now()
            .duration_since(UNIX_EPOCH)?
            .as_millis())
    }

    fn get_ip() -> Option<String> {
        let socket = UdpSocket::bind("0.0.0.0:0").ok()?;
        socket.connect(format!("http://{}:80", info::IP_TEST)).ok()?;
        socket.local_addr().ok().map(|addr| addr.ip().to_string())
    }

    pub async fn act(isp: &Isp, user: &User) -> Result<bool, reqwest::Error> {
        let client = reqwest::Client::new();

        let full_acc = format!("{}@{}",user.account,isp.domain);

        let ip = if let Some(ip) = get_ip() {
            ip
        } else {
            eprintln!("#201");
            eprintln!("FAILED(Usr): Cannot get ipv4!");
            return Ok(false)
        };

        let time = if let Ok(time) = timestamp() {
            time
        } else {
            eprintln!("#200");
            eprintln!("FAILED(Sys): Wrong system time!");
            return Ok(false)
        };

        let cl_param = super::info::ClParam{
            username: full_acc.clone(),
            ip: ip.to_string(),
            double_stack: "0".to_string(),
            timestamp: time.to_string()
        };
        let cl_url = format!("http://{}/{}:{}", isp.auth_add, info::CL_ADDRESS, isp.auth_port);
        let cl_response = client.get(&cl_url).query(&cl_param).send().await?;

        if cl_response.status().is_success() {
            let rp_map: info::ClResponse = cl_response.json().await?;
            let challenge = if let Some(cl) = rp_map.challenge {
                cl
            } else {
                eprintln!("#203");
                eprintln!("FAILED(Sys): Connot get challenge code!");
                return Ok(false)
            };

            let post_param = info::PostParam {
                action: "login".to_string(),
                username: full_acc.clone(),
                password: "".to_string(),
                ac_id: "1".to_string(),
                ip,
                double_stack: "0".to_string(),
                n: "200".to_string(),
                r#type: "1".to_string(),
                os: "Windows 11".to_string(),
                name: "Windows".to_string(),
                info: "".to_string(),
                chksum: "".to_string()
            };

            let post_url = format!("http://{}/{}:{}", isp.auth_add, info::AUTH_ADDRESS, isp.auth_port);
            let post_response = client.post(&post_url).form(&post_param).send().await?;

            if post_response.status().is_success() {
                println!("#901");
                println!("OK: Auth succeed.");
                return Ok(true)
            }

            eprintln!("#204");
            eprintln!("FAILED(Usr|Self): Auth not pass!");
            return Ok(false)
        }

        eprintln!("#202");
        eprintln!("FAILED(Usr|Self): Cannot connect challenge server!");
        Ok(false)
    }
}

mod sec{
    pub fn pw_md5(){

    }

    pub fn info_srbx(){

    }

    pub fn chksum_sh1(){

    }
}