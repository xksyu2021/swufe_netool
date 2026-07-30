mod wlan;
mod ipc;

#[tokio::main]
async fn main() {
    wlan::native::core::connect(&wlan::share::isp::TEST).await.expect("Error");
}