pub mod core{
    use crate::wlan::share::frame::{Isp, AuthType, User};
    use windows::{
        Devices::WiFi::WiFiAdapter,
        core::Result
    };
    use windows::Devices::WiFi::{WiFiConnectionStatus, WiFiReconnectionKind};

    pub async fn connect(isp: &Isp, user: &User) -> Result<bool>{
        match isp.auth_type {
            AuthType::OpenAndWeb => conn_op_web(isp,user).await,
            AuthType::Edu => conn_edu(isp,user).await,
            _ => Ok(false)
        }
    }

    async fn conn_op_web(isp: &Isp, user: &User) -> Result<bool>{
        let adapters = WiFiAdapter::FindAllAdaptersAsync()?.await?;
        if adapters.Size() == Ok(0) {
            eprintln!("#100");
            eprintln!("FAILED(Sys): No WiFi adapter!");
            return Ok(false)
        }

        let adapter = adapters.GetAt(0)?;
        adapter.ScanAsync()?.await?;

        let networks = adapter.NetworkReport()?.AvailableNetworks()?;
        for network in networks{
            if network.Ssid()? == isp.ssid {
                let response =
                    adapter.ConnectAsync(&network,WiFiReconnectionKind::Automatic)?.await?;

                if response.ConnectionStatus()? == WiFiConnectionStatus::Success {
                    println!("#900");
                    println!("OK: Native connection succeed.");

                    return match super::super::auth::core::act(isp, user).await {
                        Ok(_) => Ok(true),
                        Err(_) => Ok(false)
                    }
                }

                eprintln!("#102");
                eprintln!("FAILED(User|Self): Connection failed!");
                return Ok(false);
            }
        }

        eprintln!("#101");
        eprintln!("FAILED(User): No Target Wifi!");
        Ok(false)
    }

    async fn conn_edu(isp: &Isp, user: &User) -> Result<bool>{
        Ok(true)
    }
}