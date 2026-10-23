# PoseaAppFilter for pfSense &bull; Katman 7 (L7) Uygulama Denetimi & Protokol Filtresi

PoseaAppFilter; YouTube, Netflix, TikTok, Roblox, Steam, BitTorrent, Tor gibi popüler uygulamaları tek tıkla ağ genelinde engellemenizi sağlayan, uygulama CDN ve protokol imzalarını derinlemesine denetleyen bağımsız bir pfSense paketidir.

## 🚀 Özellikler
- **Uygulama Kataloğu:** Video/Streaming, Oyunlar, P2P/Torrent, Bypass/VPN, Sosyal Medya, Kripto Borsaları.
- **Hızlı Şablonlar:** Tümüne İzin Ver, Standart Güvenlik (P2P/Tor/TikTok/Oyun Engelle), Tümünü Engelle.
- **Özel Uygulama İmzası:** Dilediğiniz yeni uygulama ve alan adı gruplarını kolayca ekleyebilme.
- **Aç/Kapa Anahtarı:** İçindeki enable seçeneği ile dilediğiniz zaman tek tıkla aktif veya pasif edilebilir.

## 📦 Kurulum (Tek Komut)
pfSense SSH konsolunda root olarak çalıştırın:
```sh
curl -sSL https://raw.githubusercontent.com/bysinbin/pfsense-poseaappfilter/main/scripts/install.sh | sh
```
Kurulum tamamlandığında pfSense WebGUI üzerinde **Services > PoseaApp (L7 Filtre)** menüsü belirecektir.
