<?php
/*
 * poseaapp.inc - PoseaApp Layer 7 Application Control, VPN & Protocol Filtering
 * Part of Posea Security Suite
 */

require_once("config.inc");
require_once("filter.inc");

define('POSEAAPP_CONF_FILE', '/var/etc/poseafilter/poseaapp_config.json');
define('POSEAAPP_CONF_BACKUP', '/conf/poseaapp_config.json');
define('POSEAAPP_BLOCKED_FILE', '/var/etc/poseafilter/app_blocked_domains.txt');

function poseaapp_get_catalog() {
    return [
        'ai_tools' => [
            'title' => 'Yapay Zekâ (AI) & LLM Asistanları',
            'icon' => 'fa-solid fa-robot',
            'color' => '#8b5cf6',
            'apps' => [
                'chatgpt' => [
                    'name' => 'ChatGPT (OpenAI)',
                    'domains' => ['openai.com', 'chatgpt.com', 'chat.openai.com', 'oaistatic.com', 'oaiusercontent.com', 'cdn.oaistatic.com', 'api.openai.com'],
                    'ports' => '443',
                    'descr' => 'OpenAI ChatGPT web arayüzü ve API servisleri',
                ],
                'claude' => [
                    'name' => 'Claude (Anthropic)',
                    'domains' => ['claude.ai', 'anthropic.com', 'cdn.anthropic.com', 'api.anthropic.com'],
                    'ports' => '443',
                    'descr' => 'Anthropic Claude yapay zeka asistanı',
                ],
                'copilot' => [
                    'name' => 'Microsoft Copilot',
                    'domains' => ['copilot.microsoft.com', 'sydney.bing.com', 'copilot.cloud.microsoft'],
                    'ports' => '443',
                    'descr' => 'Microsoft Copilot ve Bing AI asistanı',
                ],
                'deepseek' => [
                    'name' => 'DeepSeek AI',
                    'domains' => ['deepseek.com', 'chat.deepseek.com', 'api.deepseek.com', 'cdn.deepseek.com'],
                    'ports' => '443',
                    'descr' => 'DeepSeek sohbet ve API platformu',
                ],
                'gemini' => [
                    'name' => 'Google Gemini',
                    'domains' => ['gemini.google.com', 'bard.google.com', 'generativelanguage.googleapis.com'],
                    'ports' => '443',
                    'descr' => 'Google Gemini ve Bard yapay zeka servisi',
                ],
                'grok' => [
                    'name' => 'Grok (xAI)',
                    'domains' => ['grok.com', 'x.ai', 'api.x.ai'],
                    'ports' => '443',
                    'descr' => 'Elon Musk xAI Grok yapay zeka sistemi',
                ],
                'manus' => [
                    'name' => 'Manus AI',
                    'domains' => ['manus.im', 'manus.ai'],
                    'ports' => '443',
                    'descr' => 'Manus AI otonom ajan ve çalışma alanı',
                ],
                'meta_ai' => [
                    'name' => 'Meta AI (Llama)',
                    'domains' => ['meta.ai', 'imagine.meta.com'],
                    'ports' => '443',
                    'descr' => 'Meta AI ve Imagine görsel üretim platformu',
                ],
                'perplexity' => [
                    'name' => 'Perplexity AI',
                    'domains' => ['perplexity.ai', 'pplx.ai', 'api.perplexity.ai'],
                    'ports' => '443',
                    'descr' => 'Perplexity yapay zeka arama motoru',
                ],
            ],
        ],
        'vpn_tunnels' => [
            'title' => 'VPN, Tünelleme & Anonimleşme (Bypass)',
            'icon' => 'fa-solid fa-shield-halved',
            'color' => '#ef4444',
            'apps' => [
                'nordvpn' => [
                    'name' => 'NordVPN',
                    'domains' => ['nordvpn.com', 'nordcdn.com', 'nordaccount.com'],
                    'ports' => '1194,443',
                    'descr' => 'NordVPN tünelleme ve sunucu ağı',
                ],
                'expressvpn' => [
                    'name' => 'ExpressVPN',
                    'domains' => ['expressvpn.com', 'expvpn.com', 'expressvpn.net', 'xv-services.net'],
                    'ports' => '1194,443',
                    'descr' => 'ExpressVPN ticari tünel ağı',
                ],
                'protonvpn' => [
                    'name' => 'ProtonVPN',
                    'domains' => ['protonvpn.com', 'proton.me', 'protonstatus.com', 'protonvpn.net', 'api.protonvpn.ch'],
                    'ports' => '1194,51820',
                    'descr' => 'Proton İsviçre şifreli VPN ağı',
                ],
                'surfshark' => [
                    'name' => 'Surfshark',
                    'domains' => ['surfshark.com', 'surfsharkdns.com', 'shark-services.com'],
                    'ports' => '1194,51820',
                    'descr' => 'Surfshark VPN ve sınırsız cihaz ağı',
                ],
                'openvpn' => [
                    'name' => 'OpenVPN Tünelleri',
                    'domains' => ['openvpn.net', 'openvpn.com', 'my.openvpn.com'],
                    'ports' => '1194,943',
                    'descr' => 'Standart OpenVPN UDP/TCP protokol trafiği',
                ],
                'wireguard' => [
                    'name' => 'WireGuard Tünelleri',
                    'domains' => ['wireguard.com'],
                    'ports' => '51820',
                    'descr' => 'Modern WireGuard UDP şifreli tünelleme',
                ],
                'tor' => [
                    'name' => 'Tor Project & Köprüler',
                    'domains' => ['torproject.org', 'onion', 'tor2web.org', 'tor2web.io', 'bridges.torproject.org'],
                    'ports' => '9001,9050,9150',
                    'descr' => 'Tor anonim yönlendirme ve onion ağı',
                ],
                'warp' => [
                    'name' => 'Cloudflare WARP (1.1.1.1)',
                    'domains' => ['cloudflareclient.com', 'warp.plus', '1.1.1.1', 'cloudflare-gateway.com', 'cloudflare.com'],
                    'ports' => '2408,500,4500,1701',
                    'descr' => 'Cloudflare 1.1.1.1 WARP tüneli',
                ],
                'psiphon' => [
                    'name' => 'Psiphon & UltraSurf',
                    'domains' => ['psiphon.ca', 'psiphon3.com', 'psiphon.me', 'ultrasurf.us'],
                    'ports' => '443,80,1080',
                    'descr' => 'Sansür ve filtre atlatma proxy yazılımları',
                ],
                'tailscale' => [
                    'name' => 'Tailscale Mesh VPN',
                    'domains' => ['tailscale.com', 'login.tailscale.com', 'derp.tailscale.com'],
                    'ports' => '41641,3478',
                    'descr' => 'Tailscale P2P WireGuard mesh ağı',
                ],
                'windscribe' => [
                    'name' => 'Windscribe VPN',
                    'domains' => ['windscribe.com', 'assets.windscribe.com'],
                    'ports' => '1194,443,54783',
                    'descr' => 'Windscribe güvenlik ve gizlilik tüneli',
                ],
                'hotspotshield' => [
                    'name' => 'Hotspot Shield',
                    'domains' => ['hotspotshield.com', 'anchorfree.com', 'hsselite.com'],
                    'ports' => '443,1194',
                    'descr' => 'Hotspot Shield Hydra protokolü ve tünelleri',
                ],
                'turbovpn' => [
                    'name' => 'TurboVPN',
                    'domains' => ['turbovpn.com', 'inconnecting.com'],
                    'ports' => '443,1194',
                    'descr' => 'TurboVPN mobil ve masaüstü proxy tüneli',
                ],
                'cyberghost' => [
                    'name' => 'CyberGhost VPN',
                    'domains' => ['cyberghostvpn.com', 'cg-services.net'],
                    'ports' => '1194,443',
                    'descr' => 'CyberGhost şifreli bağlantı ağı',
                ],
                'icloud_private_relay' => [
                    'name' => 'Apple iCloud Private Relay',
                    'domains' => ['mask.icloud.com', 'mask-h2.icloud.com', 'mask-api.fe.apple-dns.net'],
                    'ports' => '443',
                    'descr' => 'Apple Safari gizli röle ve şifreleme servisi',
                ],
                'privacy' => [
                    'name' => 'Privacy.com',
                    'domains' => ['privacy.com', 'api.privacy.com'],
                    'ports' => '443',
                    'descr' => 'Anonim sanal kart ve ödeme gizleme platformu',
                ],
                'proton' => [
                    'name' => 'Proton (Mail, VPN, Drive)',
                    'domains' => ['proton.me', 'protonmail.com', 'protonvpn.com', 'protonstatus.com', 'proton.ch'],
                    'ports' => '443,1194,51820',
                    'descr' => 'Proton şifreli iletişim ve depolama ekosistemi',
                ],
            ],
        ],
        'dating' => [
            'title' => 'Arkadaşlık & Tanışma (Dating)',
            'icon' => 'fa-solid fa-heart',
            'color' => '#ec4899',
            'apps' => [
                'tinder' => [
                    'name' => 'Tinder',
                    'domains' => ['tinder.com', 'gotinder.com', 'tindersparks.com', 'tinder.app.link'],
                    'ports' => '443',
                    'descr' => 'Tinder tanışma ve arkadaşlık platformu',
                ],
                'plenty_of_fish' => [
                    'name' => 'Plenty of Fish (POF)',
                    'domains' => ['pof.com', 'plentyoffish.com'],
                    'ports' => '443',
                    'descr' => 'Plenty of Fish uluslararası tanışma servisi',
                ],
                'wizz' => [
                    'name' => 'Wizz App',
                    'domains' => ['wizzapp.com', 'wizz.gg'],
                    'ports' => '443',
                    'descr' => 'Wizz genç tanışma ve sosyal sohbet uygulaması',
                ],
            ],
        ],
        'gambling' => [
            'title' => 'Bahis, Kumar & Şans Oyunları',
            'icon' => 'fa-solid fa-dice',
            'color' => '#f59e0b',
            'apps' => [
                'betano' => [
                    'name' => 'Betano',
                    'domains' => ['betano.com', 'betano.de', 'betano.pt', 'betano.bet.br', 'betano.cz', 'betano.ro'],
                    'ports' => '443',
                    'descr' => 'Betano çevrimiçi spor bahisleri ve casino',
                ],
                'betfair' => [
                    'name' => 'Betfair',
                    'domains' => ['betfair.com', 'betfair.es', 'betfair.it', 'betfairimages.com'],
                    'ports' => '443',
                    'descr' => 'Betfair bahis borsası ve kumar platformu',
                ],
                'betway' => [
                    'name' => 'Betway',
                    'domains' => ['betway.com', 'betway.co.za', 'betway.de', 'betway.es', 'betway.it'],
                    'ports' => '443',
                    'descr' => 'Betway küresel bahis ve casino servisi',
                ],
                'blaze' => [
                    'name' => 'Blaze Casino',
                    'domains' => ['blaze.com', 'blaze-1.com', 'blaze-2.com', 'blaze.trade'],
                    'ports' => '443',
                    'descr' => 'Blaze Crash ve çevrimiçi kumarhane',
                ],
            ],
        ],
        'gaming' => [
            'title' => 'Çevrimiçi Oyunlar & Platformlar',
            'icon' => 'fa-solid fa-gamepad',
            'color' => '#6366f1',
            'apps' => [
                'steam' => [
                    'name' => 'Steam (Valve)',
                    'domains' => ['steampowered.com', 'steamcommunity.com', 'steamstatic.com', 'steamserver.net'],
                    'ports' => '27015:27030,443',
                    'descr' => 'Valve Steam mağaza, topluluk ve oyun indirme sunucuları',
                ],
                'epicgames' => [
                    'name' => 'Epic Games (Fortnite)',
                    'domains' => ['epicgames.com', 'unrealengine.com', 'fortnite.com', 'ol.epicgames.com'],
                    'ports' => '443,5222',
                    'descr' => 'Epic Games Launcher ve Fortnite sunucuları',
                ],
                'riotgames' => [
                    'name' => 'Riot Games',
                    'domains' => ['riotgames.com', 'riotcdn.net', 'rgpub.io'],
                    'ports' => '443',
                    'descr' => 'Riot Games istemcisi ve doğrulama ağları',
                ],
                'leagueoflegends' => [
                    'name' => 'League of Legends (LoL)',
                    'domains' => ['leagueoflegends.com', 'pvp.net'],
                    'ports' => '2099,5223,5000:5500',
                    'descr' => 'Riot League of Legends oyun ve eşleştirme sunucuları',
                ],
                'valorant' => [
                    'name' => 'Valorant',
                    'domains' => ['playvalorant.com', 'valorant.secure.dyn.riotcdn.net'],
                    'ports' => '7000:8000',
                    'descr' => 'Valorant taktiksel FPS oyunu ve ses sunucuları',
                ],
                'battle_net' => [
                    'name' => 'Battle.net / Blizzard',
                    'domains' => ['battle.net', 'blizzard.com', 'battlenet.com'],
                    'ports' => '1119,3724',
                    'descr' => 'Blizzard Entertainment Battle.net istemcisi',
                ],
                'activision_blizzard' => [
                    'name' => 'Activision (Call of Duty)',
                    'domains' => ['activision.com', 'callofduty.com', 'demonware.net'],
                    'ports' => '3074,443',
                    'descr' => 'Activision Call of Duty ve Demonware oyun sunucuları',
                ],
                'electronic_arts' => [
                    'name' => 'Electronic Arts (EA / EA Sports)',
                    'domains' => ['ea.com', 'eadev.ea.com', 'eaassets-a.akamaihd.net'],
                    'ports' => '443',
                    'descr' => 'EA App ve EA Sports çevrimiçi hizmetleri',
                ],
                'origin' => [
                    'name' => 'EA Origin',
                    'domains' => ['origin.com', 'dm.origin.com'],
                    'ports' => '443',
                    'descr' => 'Eski EA Origin mağazası ve indirme platformu',
                ],
                'ubisoft' => [
                    'name' => 'Ubisoft Connect (Uplay)',
                    'domains' => ['ubisoft.com', 'uplay.com', 'ubisoftconnect.com'],
                    'ports' => '443,14000',
                    'descr' => 'Ubisoft Connect ve oyun sunucu ağı',
                ],
                'roblox' => [
                    'name' => 'Roblox',
                    'domains' => ['roblox.com', 'rbxcdn.com', 'roblox.com.edgesuite.net'],
                    'ports' => '443,49152:65535',
                    'descr' => 'Roblox çok oyunculu oyun platformu',
                ],
                'minecraft' => [
                    'name' => 'Minecraft (Mojang)',
                    'domains' => ['minecraft.net', 'mojang.com', 'minecraftservices.com'],
                    'ports' => '25565,443',
                    'descr' => 'Minecraft sunucu ve hesap doğrulama servisleri',
                ],
                'rockstar_games' => [
                    'name' => 'Rockstar Games (GTA V)',
                    'domains' => ['rockstargames.com', 'socialclub.rockstargames.com'],
                    'ports' => '6672,61455:61458',
                    'descr' => 'Rockstar Games Social Club ve GTA Online',
                ],
                'gog' => [
                    'name' => 'GOG.com (Galaxy)',
                    'domains' => ['gog.com', 'gog-statics.com'],
                    'ports' => '443',
                    'descr' => 'CD Projekt GOG Galaxy oyun platformu',
                ],
                'io_interactive' => [
                    'name' => 'IO Interactive (Hitman)',
                    'domains' => ['ioi.dk', 'hitman.com'],
                    'ports' => '443',
                    'descr' => 'IO Interactive Hitman çevrimiçi sunucuları',
                ],
                'nintendo' => [
                    'name' => 'Nintendo Network / Switch',
                    'domains' => ['nintendo.com', 'nintendo.net', 'nintendo.co.jp', 'nintendowifi.net'],
                    'ports' => '443',
                    'descr' => 'Nintendo Switch eShop ve çevrimiçi oyun servisi',
                ],
                'playstation' => [
                    'name' => 'PlayStation Network (PSN)',
                    'domains' => ['playstation.com', 'sonyentertainmentnetwork.com', 'playstation.net', 'playstationnetwork.com'],
                    'ports' => '3478:3480,443',
                    'descr' => 'Sony PS4 / PS5 PlayStation Network ekosistemi',
                ],
                'xbox_live' => [
                    'name' => 'Xbox Live (Microsoft)',
                    'domains' => ['xbox.com', 'xboxlive.com', 'xboxab.com'],
                    'ports' => '3074,88,500',
                    'descr' => 'Microsoft Xbox konsol ve PC Game Pass ağı',
                ],
                'wargaming' => [
                    'name' => 'Wargaming (World of Tanks)',
                    'domains' => ['wargaming.net', 'worldoftanks.eu', 'worldoftanks.com', 'worldofwarships.eu'],
                    'ports' => '443,20014:20020',
                    'descr' => 'World of Tanks ve World of Warships sunucuları',
                ],
                'warner_bros_games' => [
                    'name' => 'Warner Bros Games',
                    'domains' => ['wbgames.com', 'warnerbrosgames.com', 'warnerbros.com'],
                    'ports' => '443',
                    'descr' => 'Warner Bros oyunları ve çevrimiçi servisleri',
                ],
            ],
        ],
        'cloud_storage' => [
            'title' => 'Bulut Depolama, Dosya & Fotoğraf Paylaşımı',
            'icon' => 'fa-solid fa-cloud',
            'color' => '#0ea5e9',
            'apps' => [
                'box' => [
                    'name' => 'Box.com',
                    'domains' => ['box.com', 'boxcdn.net', 'box.net'],
                    'ports' => '443',
                    'descr' => 'Kurumsal bulut depolama ve dosya paylaşım platformu',
                ],
                'dropbox' => [
                    'name' => 'Dropbox',
                    'domains' => ['dropbox.com', 'dropboxapi.com', 'dropboxstatic.com', 'dropboxusercontent.com'],
                    'ports' => '443',
                    'descr' => 'Dropbox dosya senkronizasyon ve depolama ağı',
                ],
                'flickr' => [
                    'name' => 'Flickr',
                    'domains' => ['flickr.com', 'staticflickr.com', 'flic.kr'],
                    'ports' => '443',
                    'descr' => 'Fotoğraf ve video barındırma topluluğu',
                ],
                'imgur' => [
                    'name' => 'Imgur',
                    'domains' => ['imgur.com', 'i.imgur.com', 'imgurinc.com'],
                    'ports' => '443',
                    'descr' => 'Görsel yükleme ve paylaşım servisi',
                ],
                'bittorrent' => [
                    'name' => 'BitTorrent (P2P Torrent)',
                    'domains' => ['torrent', 'utorrent.com', 'bittorrent.com', 'thepiratebay.org', '1337x.to', 'yts.mx', 'rarbg.to'],
                    'ports' => '6881:6889',
                    'descr' => 'P2P dosya paylaşım protokolü ve istemcileri',
                ],
            ],
        ],
        'messaging' => [
            'title' => 'Anlık Mesajlaşma, Arama & İşbirliği',
            'icon' => 'fa-solid fa-comments',
            'color' => '#10b981',
            'apps' => [
                'whatsapp' => [
                    'name' => 'WhatsApp',
                    'domains' => ['whatsapp.com', 'whatsapp.net', 'web.whatsapp.com', 'wa.me'],
                    'ports' => '443,5222',
                    'descr' => 'Meta WhatsApp Web ve mobil anlık mesajlaşma',
                ],
                'telegram' => [
                    'name' => 'Telegram',
                    'domains' => ['telegram.org', 't.me', 'telegram.me', 'tdesktop.com'],
                    'ports' => '443,80',
                    'descr' => 'Telegram şifreli mesajlaşma ve dosya transferi',
                ],
                'signal' => [
                    'name' => 'Signal Messenger',
                    'domains' => ['signal.org', 'whispersystems.org', 'updates2.signal.org'],
                    'ports' => '443',
                    'descr' => 'Signal uçtan uca şifreli mesajlaşma',
                ],
                'microsoft_teams' => [
                    'name' => 'Microsoft Teams',
                    'domains' => ['teams.microsoft.com', 'teams.live.com', 'teams.cdn.live.net'],
                    'ports' => '443',
                    'descr' => 'Microsoft Teams kurumsal görüntülü toplantı ve sohbet',
                ],
                'slack' => [
                    'name' => 'Slack',
                    'domains' => ['slack.com', 'slack-edge.com', 'slack-msgs.com', 'slackb.com'],
                    'ports' => '443',
                    'descr' => 'Slack işyeri mesajlaşma ve kanal platformu',
                ],
                'skype' => [
                    'name' => 'Skype',
                    'domains' => ['skype.com', 'skypeassets.com', 'pipe.skype.com'],
                    'ports' => '443',
                    'descr' => 'Microsoft Skype sesli ve görüntülü arama',
                ],
                'discord' => [
                    'name' => 'Discord',
                    'domains' => ['discord.com', 'discordapp.com', 'discord.gg', 'discordapp.net', 'discord.media'],
                    'ports' => '443,50000:65535',
                    'descr' => 'Discord sesli iletişim, yayın ve topluluk kanalları',
                ],
                'viber' => [
                    'name' => 'Viber',
                    'domains' => ['viber.com', 'viber.me', 'viber-cdn.biz'],
                    'ports' => '443,5242',
                    'descr' => 'Rakuten Viber anlık mesajlaşma ve VoIP',
                ],
                'wechat' => [
                    'name' => 'WeChat',
                    'domains' => ['wechat.com', 'weixin.qq.com', 'qpic.cn', 'servicewechat.com'],
                    'ports' => '443,80',
                    'descr' => 'Tencent WeChat anlık sohbet ve ödeme servisi',
                ],
                'kakaotalk' => [
                    'name' => 'KakaoTalk',
                    'domains' => ['kakaotalk.com', 'kakao.com', 'kakaocdn.net'],
                    'ports' => '443,5223',
                    'descr' => 'KakaoTalk mesajlaşma uygulaması',
                ],
                'kik' => [
                    'name' => 'Kik Messenger',
                    'domains' => ['kik.com', 'kik.me'],
                    'ports' => '443,5222',
                    'descr' => 'Kik anonim mobil mesajlaşma',
                ],
                'olvid' => [
                    'name' => 'Olvid Messenger',
                    'domains' => ['olvid.io'],
                    'ports' => '443',
                    'descr' => 'Olvid ultra-güvenli Fransız mesajlaşma uygulaması',
                ],
                'line' => [
                    'name' => 'LINE Messenger',
                    'domains' => ['line.me', 'line-apps.com', 'line-scdn.net', 'naver.jp'],
                    'ports' => '443',
                    'descr' => 'LINE Asya anlık mesajlaşma ve arama servisi',
                ],
                'kook' => [
                    'name' => 'KOOK (Kaiheila)',
                    'domains' => ['kookapp.cn', 'kaiheila.cn'],
                    'ports' => '443',
                    'descr' => 'KOOK oyuncu sesli sohbet ve topluluk ağı',
                ],
            ],
        ],
        'ecommerce' => [
            'title' => 'E-Ticaret & Alışveriş',
            'icon' => 'fa-solid fa-cart-shopping',
            'color' => '#f97316',
            'apps' => [
                'amazon' => [
                    'name' => 'Amazon',
                    'domains' => ['amazon.com', 'amazon.com.tr', 'amazon.de', 'ssl-images-amazon.com', 'media-amazon.com', 'a.co'],
                    'ports' => '443',
                    'descr' => 'Amazon küresel e-ticaret ve alışveriş sitesi',
                ],
                'aliexpress' => [
                    'name' => 'AliExpress',
                    'domains' => ['aliexpress.com', 'aliexpress.ru', 'alicdn.com', 'alipay.com'],
                    'ports' => '443',
                    'descr' => 'Alibaba AliExpress uluslararası alışveriş pazarı',
                ],
                'ebay' => [
                    'name' => 'eBay',
                    'domains' => ['ebay.com', 'ebaystatic.com', 'ebayimg.com', 'ebay.de', 'ebay.co.uk'],
                    'ports' => '443',
                    'descr' => 'eBay açık artırma ve perakende alışveriş sitesi',
                ],
                'temu' => [
                    'name' => 'Temu',
                    'domains' => ['temu.com', 'temu.net', 'kuingsfleet.com'],
                    'ports' => '443',
                    'descr' => 'Temu indirimli sınır ötesi alışveriş uygulaması',
                ],
                'shein' => [
                    'name' => 'Shein',
                    'domains' => ['shein.com', 'sheinimg.com', 'sheincdn.com'],
                    'ports' => '443',
                    'descr' => 'Shein hızlı moda ve giyim alışveriş platformu',
                ],
                'shopee' => [
                    'name' => 'Shopee',
                    'domains' => ['shopee.com', 'shopee.com.my', 'shopeemobile.com', 'shopee.co.id', 'shopee.ph'],
                    'ports' => '443',
                    'descr' => 'Güneydoğu Asya Shopee e-ticaret ağı',
                ],
                'lazada' => [
                    'name' => 'Lazada',
                    'domains' => ['lazada.com', 'lazada.sg', 'laz-img-cdn.com'],
                    'ports' => '443',
                    'descr' => 'Lazada Asya e-ticaret mağazası',
                ],
                'mercado_libre' => [
                    'name' => 'Mercado Libre',
                    'domains' => ['mercadolibre.com', 'mercadolibre.com.ar', 'mercadolibre.com.br', 'mercadopago.com'],
                    'ports' => '443',
                    'descr' => 'Latin Amerika Mercado Libre pazaryeri',
                ],
                'xiaohongshu' => [
                    'name' => 'Xiaohongshu (RED)',
                    'domains' => ['xiaohongshu.com', 'xhscdn.com'],
                    'ports' => '443',
                    'descr' => 'RED Çin sosyal ticaret ve yaşam tarzı platformu',
                ],
                'coolapk' => [
                    'name' => 'CoolAPK',
                    'domains' => ['coolapk.com', 'coolapkmarket.com'],
                    'ports' => '443',
                    'descr' => 'CoolAPK teknoloji topluluğu ve uygulama mağazası',
                ],
            ],
        ],
        'social' => [
            'title' => 'Sosyal Medya, Topluluk & Forumlar',
            'icon' => 'fa-solid fa-share-nodes',
            'color' => '#3b82f6',
            'apps' => [
                'facebook' => [
                    'name' => 'Facebook',
                    'domains' => ['facebook.com', 'fbcdn.net', 'fbsbx.com', 'fb.me', 'm.me'],
                    'ports' => '443',
                    'descr' => 'Meta Facebook sosyal platformu',
                ],
                'instagram' => [
                    'name' => 'Instagram',
                    'domains' => ['instagram.com', 'cdninstagram.com', 'ig.me'],
                    'ports' => '443',
                    'descr' => 'Instagram fotoğraf ve Reels paylaşım ağı',
                ],
                'tiktok' => [
                    'name' => 'TikTok',
                    'domains' => ['tiktok.com', 'tiktokcdn.com', 'byteoversea.com', 'ibytedtos.com', 'tiktokv.com'],
                    'ports' => '443',
                    'descr' => 'ByteDance TikTok kısa video platformu',
                ],
                'twitter_x' => [
                    'name' => 'X (Eski Twitter)',
                    'domains' => ['twitter.com', 'x.com', 'twimg.com', 't.co'],
                    'ports' => '443',
                    'descr' => 'X mikroblog ve sosyal akış platformu',
                ],
                'linkedin' => [
                    'name' => 'LinkedIn',
                    'domains' => ['linkedin.com', 'licdn.com'],
                    'ports' => '443',
                    'descr' => 'Profesyonel iş ve kariyer ağı',
                ],
                'reddit' => [
                    'name' => 'Reddit',
                    'domains' => ['reddit.com', 'redd.it', 'redditstatic.com', 'redditmedia.com'],
                    'ports' => '443',
                    'descr' => 'Reddit topluluk ve tartışma forumları',
                ],
                'pinterest' => [
                    'name' => 'Pinterest',
                    'domains' => ['pinterest.com', 'pinimg.com'],
                    'ports' => '443',
                    'descr' => 'Görsel keşif ve pano platformu',
                ],
                'snapchat' => [
                    'name' => 'Snapchat',
                    'domains' => ['snapchat.com', 'sc-cdn.net', 'snap-dev.net'],
                    'ports' => '443',
                    'descr' => 'Snapchat anlık fotoğraf ve hikaye ağı',
                ],
                'tumblr' => [
                    'name' => 'Tumblr',
                    'domains' => ['tumblr.com', 'tmblr.co'],
                    'ports' => '443',
                    'descr' => 'Tumblr mikroblog ve yaratıcı topluluk',
                ],
                'bluesky' => [
                    'name' => 'Bluesky',
                    'domains' => ['bsky.app', 'bsky.social', 'bsky.network'],
                    'ports' => '443',
                    'descr' => 'Bluesky merkeziyetsiz sosyal ağ',
                ],
                'mastodon' => [
                    'name' => 'Mastodon',
                    'domains' => ['mastodon.social', 'joinmastodon.org'],
                    'ports' => '443',
                    'descr' => 'Fediverse açık kaynaklı federe sosyal ağ',
                ],
                'clubhouse' => [
                    'name' => 'Clubhouse',
                    'domains' => ['clubhouse.com', 'joinclubhouse.com', 'clubhouseapi.com'],
                    'ports' => '443',
                    'descr' => 'Clubhouse sesli sohbet ve oda platformu',
                ],
                '4chan' => [
                    'name' => '4chan',
                    'domains' => ['4chan.org', '4channel.org'],
                    'ports' => '443',
                    'descr' => 'Anonim görsel panosu ve forumu',
                ],
                '9gag' => [
                    'name' => '9GAG',
                    'domains' => ['9gag.com', '9cache.com'],
                    'ports' => '443',
                    'descr' => 'Mizah ve viral içerik platformu',
                ],
                '500px' => [
                    'name' => '500px',
                    'domains' => ['500px.com', '500px.net'],
                    'ports' => '443',
                    'descr' => 'Fotoğrafçılık topluluğu ve portfolyo',
                ],
                'amino' => [
                    'name' => 'Amino Apps',
                    'domains' => ['aminoapps.com', 'narvii.com'],
                    'ports' => '443',
                    'descr' => 'Amino ilgi alanlarına göre topluluk uygulaması',
                ],
                'douban' => [
                    'name' => 'Douban',
                    'domains' => ['douban.com', 'doubanio.com'],
                    'ports' => '443',
                    'descr' => 'Douban Çin kitap, film ve müzik topluluğu',
                ],
                'vk' => [
                    'name' => 'VKontakte (VK)',
                    'domains' => ['vk.com', 'userapi.com', 'vk.me'],
                    'ports' => '443',
                    'descr' => 'Rusya ve Doğu Avrupa sosyal medya platformu',
                ],
                'ok_ru' => [
                    'name' => 'OK.ru (Odnoklassniki)',
                    'domains' => ['ok.ru', 'odnoklassniki.ru'],
                    'ports' => '443',
                    'descr' => 'Odnoklassniki Rus arkadaşlık ve sosyal ağı',
                ],
                'mail_ru' => [
                    'name' => 'Mail.ru',
                    'domains' => ['mail.ru', 'mrcdn.net', 'imgsmail.ru'],
                    'ports' => '443',
                    'descr' => 'Mail.ru portal ve sosyal servisleri',
                ],
                'weibo' => [
                    'name' => 'Weibo (Sina Weibo)',
                    'domains' => ['weibo.com', 'weibo.cn', 'sinaimg.cn', 'weibo.net'],
                    'ports' => '443',
                    'descr' => 'Çin mikroblog ve sosyal ağı',
                ],
                'zhihu' => [
                    'name' => 'Zhihu',
                    'domains' => ['zhihu.com', 'zhimg.com'],
                    'ports' => '443',
                    'descr' => 'Zhihu Çin soru-cevap ve bilgi topluluğu',
                ],
                'onlyfans' => [
                    'name' => 'OnlyFans',
                    'domains' => ['onlyfans.com', 'onlyfanscdn.com'],
                    'ports' => '443',
                    'descr' => 'İçerik üretici abonelik platformu',
                ],
                'odysee' => [
                    'name' => 'Odysee (LBRY)',
                    'domains' => ['odysee.com', 'lbry.tv', 'lbry.com'],
                    'ports' => '443',
                    'descr' => 'Merkeziyetsiz video paylaşım ağı',
                ],
            ],
        ],
        'streaming' => [
            'title' => 'Video & Müzik (Streaming & Canlı Yayın)',
            'icon' => 'fa-solid fa-play',
            'color' => '#a855f7',
            'apps' => [
                'youtube' => [
                    'name' => 'YouTube',
                    'domains' => ['youtube.com', 'googlevideo.com', 'ytimg.com', 'youtu.be', 'youtube-nocookie.com'],
                    'ports' => '443',
                    'descr' => 'Google YouTube video akış ve müzik platformu',
                ],
                'netflix' => [
                    'name' => 'Netflix',
                    'domains' => ['netflix.com', 'nflxvideo.net', 'nflximg.net', 'nflxext.com', 'nflxso.net'],
                    'ports' => '443',
                    'descr' => 'Netflix dizi, film ve belgesel akış servisi',
                ],
                'disney_plus' => [
                    'name' => 'Disney+',
                    'domains' => ['disneyplus.com', 'disney-portal.my.onetrust.com', 'bamgrid.com', 'disneystreaming.com'],
                    'ports' => '443',
                    'descr' => 'The Walt Disney Company yayın servisi',
                ],
                'amazon_streaming' => [
                    'name' => 'Amazon Prime Video',
                    'domains' => ['primevideo.com', 'amazonvideo.com', 'aiv-cdn.net', 'atv-ps.amazon.com'],
                    'ports' => '443',
                    'descr' => 'Amazon Prime Video yayın ve film platformu',
                ],
                'apple_streaming' => [
                    'name' => 'Apple TV+ / Streaming',
                    'domains' => ['tv.apple.com', 'apple.com/apple-tv-plus', 'video-edge.itunes.apple.com'],
                    'ports' => '443',
                    'descr' => 'Apple TV+ ve iTunes video akış sunucuları',
                ],
                'hbo_max' => [
                    'name' => 'Max (HBO Max)',
                    'domains' => ['max.com', 'hbomax.com', 'hbo.com', 'discomax.com'],
                    'ports' => '443',
                    'descr' => 'Warner Bros Discovery Max akış platformu',
                ],
                'hulu' => [
                    'name' => 'Hulu',
                    'domains' => ['hulu.com', 'hulustream.com', 'huluim.com'],
                    'ports' => '443',
                    'descr' => 'Disney Hulu isteğe bağlı video akışı',
                ],
                'paramount_plus' => [
                    'name' => 'Paramount+',
                    'domains' => ['paramountplus.com', 'cbsaavs.com'],
                    'ports' => '443',
                    'descr' => 'Paramount Global dizi ve film servisi',
                ],
                'peacock_tv' => [
                    'name' => 'Peacock TV',
                    'domains' => ['peacocktv.com'],
                    'ports' => '443',
                    'descr' => 'NBCUniversal Peacock yayın ağı',
                ],
                'discovery_plus' => [
                    'name' => 'Discovery+',
                    'domains' => ['discoveryplus.com', 'discovery.com'],
                    'ports' => '443',
                    'descr' => 'Discovery+ belgesel ve eğlence platformu',
                ],
                'lionsgate_plus' => [
                    'name' => 'Lionsgate+ (Starz)',
                    'domains' => ['lionsgateplus.com', 'starz.com'],
                    'ports' => '443',
                    'descr' => 'Lionsgate+ ve Starz yayın kanalı',
                ],
                'crunchyroll' => [
                    'name' => 'Crunchyroll',
                    'domains' => ['crunchyroll.com', 'vrv.co'],
                    'ports' => '443',
                    'descr' => 'Sony Anime akış ve manga platformu',
                ],
                'dailymotion' => [
                    'name' => 'Dailymotion',
                    'domains' => ['dailymotion.com', 'dmcdn.net'],
                    'ports' => '443',
                    'descr' => 'Fransa merkezli video paylaşım portalı',
                ],
                'vimeo' => [
                    'name' => 'Vimeo',
                    'domains' => ['vimeo.com', 'vimeocdn.com'],
                    'ports' => '443',
                    'descr' => 'Yüksek kaliteli profesyonel video platformu',
                ],
                'twitch' => [
                    'name' => 'Twitch',
                    'domains' => ['twitch.tv', 'ttvnw.net', 'jtvnw.net'],
                    'ports' => '443',
                    'descr' => 'Amazon Twitch canlı oyun ve yayın platformu',
                ],
                'spotify' => [
                    'name' => 'Spotify',
                    'domains' => ['spotify.com', 'scdn.co', 'spclient.wg.spotify.com', 'spotifycdn.com'],
                    'ports' => '443,4070',
                    'descr' => 'Spotify dijital müzik ve podcast akış servisi',
                ],
                'spotify_video' => [
                    'name' => 'Spotify Video Streaming',
                    'domains' => ['video-ak.spotifycdn.com', 'heads4-ak.spotify.com.edgesuite.net'],
                    'ports' => '443',
                    'descr' => 'Spotify video podcast ve Canvas akış sunucuları',
                ],
                'deezer' => [
                    'name' => 'Deezer',
                    'domains' => ['deezer.com', 'dzcdn.net'],
                    'ports' => '443',
                    'descr' => 'Fransız Deezer müzik ve ses platformu',
                ],
                'tidal' => [
                    'name' => 'Tidal',
                    'domains' => ['tidal.com', 'wimptheory.com'],
                    'ports' => '443',
                    'descr' => 'Tidal Hi-Fi kayıpsız ses ve müzik servisi',
                ],
                'soundcloud' => [
                    'name' => 'SoundCloud',
                    'domains' => ['soundcloud.com', 'sndcdn.com'],
                    'ports' => '443',
                    'descr' => 'SoundCloud müzik paylaşım ve sanatçı topluluğu',
                ],
                'iheartradio' => [
                    'name' => 'iHeartRadio',
                    'domains' => ['iheart.com', 'ihr.securenetsystems.net', 'iheartmedia.com'],
                    'ports' => '443',
                    'descr' => 'iHeartRadio canlı radyo ve podcast ağı',
                ],
                'nebula' => [
                    'name' => 'Nebula Streaming',
                    'domains' => ['nebula.tv', 'watchnebula.com'],
                    'ports' => '443',
                    'descr' => 'Bağımsız yaratıcı odaklı video platformu',
                ],
                'plex' => [
                    'name' => 'Plex',
                    'domains' => ['plex.tv', 'plex.direct'],
                    'ports' => '32400,443',
                    'descr' => 'Plex kişisel medya sunucusu ve ücretsiz TV',
                ],
                'pluto_tv' => [
                    'name' => 'Pluto TV',
                    'domains' => ['pluto.tv'],
                    'ports' => '443',
                    'descr' => 'Paramount Pluto TV ücretsiz canlı yayın kanalları',
                ],
                'samsung_tv_plus' => [
                    'name' => 'Samsung TV Plus',
                    'domains' => ['samsungtvplus.com', 'samsungcloudcdn.com'],
                    'ports' => '443',
                    'descr' => 'Samsung akıllı TV ve mobil ücretsiz kanal ağı',
                ],
                'looke' => [
                    'name' => 'Looke',
                    'domains' => ['looke.com.br'],
                    'ports' => '443',
                    'descr' => 'Looke Brezilya film ve dizi yayın servisi',
                ],
                'directv_go' => [
                    'name' => 'DirecTV Go (DGO)',
                    'domains' => ['directvgo.com'],
                    'ports' => '443',
                    'descr' => 'DirecTV Go canlı televizyon ve yayın ağı',
                ],
                'globoplay' => [
                    'name' => 'Globoplay',
                    'domains' => ['globoplay.globo.com', 'globo.com'],
                    'ports' => '443',
                    'descr' => 'Globo Brezilya yayın platformu',
                ],
                'canais_globo' => [
                    'name' => 'Canais Globo',
                    'domains' => ['canaisglobo.globo.com'],
                    'ports' => '443',
                    'descr' => 'Canais Globo canlı yayın kanalları',
                ],
                'claro' => [
                    'name' => 'Claro TV+ / Video',
                    'domains' => ['clarotvmais.com.br', 'clarovideo.com'],
                    'ports' => '443',
                    'descr' => 'Claro América Móvil yayın servisi',
                ],
                'vivo_play' => [
                    'name' => 'Vivo Play',
                    'domains' => ['vivoplay.com.br'],
                    'ports' => '443',
                    'descr' => 'Telefónica Vivo Play video platformu',
                ],
                'voot' => [
                    'name' => 'Voot (JioCinema)',
                    'domains' => ['voot.com', 'jiocinema.com'],
                    'ports' => '443',
                    'descr' => 'Hindistan JioCinema / Voot dizi ve film servisi',
                ],
                'espn' => [
                    'name' => 'ESPN',
                    'domains' => ['espn.com', 'espncdn.com', 'espn.go.com'],
                    'ports' => '443',
                    'descr' => 'ESPN canlı spor yayınları ve video ağı',
                ],
                'fifa' => [
                    'name' => 'FIFA+',
                    'domains' => ['fifa.com', 'plus.fifa.com'],
                    'ports' => '443',
                    'descr' => 'FIFA+ resmi futbol maçları ve belgesel yayını',
                ],
                'bilibili' => [
                    'name' => 'Bilibili',
                    'domains' => ['bilibili.com', 'bilivideo.com', 'hdslb.com'],
                    'ports' => '443',
                    'descr' => 'Çin Bilibili video ve anime topluluğu',
                ],
                'iqiyi' => [
                    'name' => 'iQIYI',
                    'domains' => ['iqiyi.com', 'iq.com', '71.am'],
                    'ports' => '443',
                    'descr' => 'Baidu iQIYI Asya drama ve dizi platformu',
                ],
                'qq' => [
                    'name' => 'Tencent QQ Video',
                    'domains' => ['qq.com', 'v.qq.com', 'tencent.com'],
                    'ports' => '443',
                    'descr' => 'Tencent Video ve QQ medya servisi',
                ],
                'yy' => [
                    'name' => 'YY Live',
                    'domains' => ['yy.com', 'yystatic.com'],
                    'ports' => '443',
                    'descr' => 'YY Çin canlı yayın ve interaktif video ağı',
                ],
                'bigo_live' => [
                    'name' => 'Bigo Live',
                    'domains' => ['bigo.tv', 'bigo.sg'],
                    'ports' => '443',
                    'descr' => 'Bigo Live küresel canlı yayın ve sohbet platformu',
                ],
                'rakuten_viki' => [
                    'name' => 'Rakuten Viki',
                    'domains' => ['viki.com', 'viki.io'],
                    'ports' => '443',
                    'descr' => 'Rakuten Viki Asya dizileri ve altyazılı içerik',
                ],
            ],
        ],
        'system_stores' => [
            'title' => 'Uygulama Mağazaları & Sistem Servisleri',
            'icon' => 'fa-solid fa-laptop-code',
            'color' => '#14b8a6',
            'apps' => [
                'play_store' => [
                    'name' => 'Google Play Store',
                    'domains' => ['play.google.com', 'android.clients.google.com', 'market.android.com'],
                    'ports' => '443',
                    'descr' => 'Android Google Play uygulama indirme ve güncelleme',
                ],
                'nvidia' => [
                    'name' => 'NVIDIA (GeForce NOW)',
                    'domains' => ['nvidia.com', 'nvidiagrid.net', 'geforcenow.com', 'nvidia.custhelp.com'],
                    'ports' => '443,49000:49010',
                    'descr' => 'NVIDIA GeForce sürücü indirmeleri ve bulut oyun ağı',
                ],
            ],
        ],
        'remote_access' => [
            'title' => 'Uzak Masaüstü & Shadow IT (Remote Control)',
            'icon' => 'fa-solid fa-desktop',
            'color' => '#64748b',
            'apps' => [
                'anydesk' => [
                    'name' => 'AnyDesk',
                    'domains' => ['anydesk.com', 'net.anydesk.com', 'boot.anydesk.com'],
                    'ports' => '7070,6568,443',
                    'descr' => 'AnyDesk uzaktan kontrol ve dosya aktarımı',
                ],
                'teamviewer' => [
                    'name' => 'TeamViewer',
                    'domains' => ['teamviewer.com', 'tmvm.net', 'router.teamviewer.com'],
                    'ports' => '5938,443',
                    'descr' => 'TeamViewer uzak bağlantı ve toplantı yazılımı',
                ],
                'rustdesk' => [
                    'name' => 'RustDesk',
                    'domains' => ['rustdesk.com'],
                    'ports' => '21115:21119',
                    'descr' => 'Açık kaynaklı P2P uzak masaüstü aracı',
                ],
                'chrome_rdp' => [
                    'name' => 'Chrome Remote Desktop',
                    'domains' => ['remotedesktop.google.com', 'chromoting-host.google.com'],
                    'ports' => '443',
                    'descr' => 'Google Chrome tarayıcı tabanlı uzak masaüstü',
                ],
            ],
        ],
        'crypto' => [
            'title' => 'Kripto Para Borsaları & Madencilik',
            'icon' => 'fa-solid fa-coins',
            'color' => '#eab308',
            'apps' => [
                'binance' => [
                    'name' => 'Binance',
                    'domains' => ['binance.com', 'binance.me', 'binance.info', 'binance.cloud', 'bnbstatic.com'],
                    'ports' => '443',
                    'descr' => 'Küresel kripto varlık alım satım platformu',
                ],
                'btcturk' => [
                    'name' => 'BtcTurk',
                    'domains' => ['btcturk.com', 'btcturk.pro'],
                    'ports' => '443',
                    'descr' => 'Türkiye yerel kripto borsası',
                ],
                'paribu' => [
                    'name' => 'Paribu',
                    'domains' => ['paribu.com'],
                    'ports' => '443',
                    'descr' => 'Paribu kripto para işlem platformu',
                ],
                'coinbase' => [
                    'name' => 'Coinbase',
                    'domains' => ['coinbase.com'],
                    'ports' => '443',
                    'descr' => 'Coinbase küresel kripto borsası',
                ],
                'kraken' => [
                    'name' => 'Kraken',
                    'domains' => ['kraken.com'],
                    'ports' => '443',
                    'descr' => 'Kraken dijital varlık borsası',
                ],
                'kucoin' => [
                    'name' => 'KuCoin',
                    'domains' => ['kucoin.com'],
                    'ports' => '443',
                    'descr' => 'KuCoin kripto ticaret platformu',
                ],
            ],
        ],
    ];
}

function poseaapp_get_default_config() {
    $catalog = poseaapp_get_catalog();
    $apps = [];
    foreach ($catalog as $cat) {
        foreach ($cat['apps'] as $aid => $ainfo) {
            $default_act = in_array($aid, ['tiktok', 'roblox', 'bittorrent', 'tor', 'warp', 'psiphon', 'binance']) ? 'block' : 'allow';
            $apps[$aid] = [
                'action' => $default_act
            ];
        }
    }
    return [
        'enabled' => false, // Default is disabled (pasif)
        'log_blocks' => true,
        'apps' => $apps,
        'client_rules' => [], // Granular rule-based application control (AD, IP, Subnet, Alias)
        'custom_apps' => []
    ];
}

function poseaapp_get_config() {
    $conf = null;
    if (file_exists(POSEAAPP_CONF_FILE)) {
        $raw = @file_get_contents(POSEAAPP_CONF_FILE);
        $conf = json_decode($raw, true);
    }
    if (!is_array($conf) && file_exists(POSEAAPP_CONF_BACKUP)) {
        $raw = @file_get_contents(POSEAAPP_CONF_BACKUP);
        $conf = json_decode($raw, true);
    }
    if (!is_array($conf)) {
        $conf = poseaapp_get_default_config();
        poseaapp_save_config($conf);
    }
    if (!isset($conf['client_rules']) || !is_array($conf['client_rules'])) {
        $conf['client_rules'] = [];
    }
    return $conf;
}

function poseaapp_save_config($conf) {
    if (!is_dir(dirname(POSEAAPP_CONF_FILE))) {
        @mkdir(dirname(POSEAAPP_CONF_FILE), 0755, true);
    }
    $json = json_encode($conf, JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE);
    @file_put_contents(POSEAAPP_CONF_FILE, $json);
    @file_put_contents(POSEAAPP_CONF_BACKUP, $json);
}

function poseaapp_get_ad_metadata() {
    global $config;
    $cache_file = '/tmp/poseaapp_ad_metadata.json';
    if (file_exists($cache_file) && (time() - filemtime($cache_file) < 300)) {
        $cached = @json_decode(file_get_contents($cache_file), true);
        if ($cached && !empty($cached['ad_groups'])) {
            return $cached;
        }
    }

    $res = [
        'aliases' => [],
        'ad_groups' => [],
        'ad_users' => [],
        'lan_clients' => []
    ];

    // 1. pfSense Host & Network Aliases
    if (isset($config['aliases']['alias']) && is_array($config['aliases']['alias'])) {
        foreach ($config['aliases']['alias'] as $a) {
            if (in_array($a['type'], ['host', 'network'])) {
                $res['aliases'][] = [
                    'name' => $a['name'],
                    'type' => $a['type'],
                    'descr' => $a['descr'] ?? '',
                    'address' => $a['address'] ?? ''
                ];
            }
        }
    }

    // 2. Active Directory Groups and Users
    $groups_found = [];
    $users_found = [];

    if (file_exists('/usr/local/pkg/posea_ad_sso.inc')) {
        require_once('/usr/local/pkg/posea_ad_sso.inc');
        if (function_exists('posea_sso_get_sessions')) {
            $sessions = posea_sso_get_sessions();
            foreach ($sessions as $ip => $s) {
                if (!empty($s['groups']) && is_array($s['groups'])) {
                    foreach ($s['groups'] as $g) {
                        $groups_found[$g] = true;
                    }
                }
                if (!empty($s['department'])) {
                    $groups_found[$s['department']] = true;
                }
                if (!empty($s['username']) && $s['username'] !== 'anonymous') {
                    $users_found[$s['username']] = [
                        'username' => $s['username'],
                        'display_name' => $s['display_name'] ?? $s['username'],
                        'ip' => $ip,
                        'hostname' => $s['hostname'] ?? ''
                    ];
                }
            }
        }

        if (function_exists('posea_sso_get_config') && function_exists('posea_sso_ldap_connect') && function_exists('posea_sso_ldap_bind')) {
            $cfg = posea_sso_get_config();
            $ldap = posea_sso_ldap_connect($cfg);
            if ($ldap && posea_sso_ldap_bind($ldap, $cfg['bind_user'] ?? '', $cfg['bind_password'] ?? '')) {
                $sr = @ldap_search($ldap, $cfg['base_dn'], '(|(objectClass=group)(objectCategory=group))', ['cn']);
                if ($sr) {
                    $entries = @ldap_get_entries($ldap, $sr);
                    for ($i = 0; $i < ($entries['count'] ?? 0); $i++) {
                        $cn = $entries[$i]['cn'][0] ?? '';
                        if ($cn) $groups_found[$cn] = true;
                    }
                }

                $sr_u = @ldap_search($ldap, $cfg['base_dn'], '(&(objectCategory=person)(objectClass=user)(!(userAccountControl:1.2.840.113556.1.4.803:=2)))', ['sAMAccountName', 'displayName']);
                if ($sr_u) {
                    $entries_u = @ldap_get_entries($ldap, $sr_u);
                    for ($i = 0; $i < ($entries_u['count'] ?? 0); $i++) {
                        $sam = $entries_u[$i]['samaccountname'][0] ?? '';
                        $dn = $entries_u[$i]['displayname'][0] ?? $sam;
                        if ($sam && !isset($users_found[$sam])) {
                            $users_found[$sam] = [
                                'username' => $sam,
                                'display_name' => $dn,
                                'ip' => '',
                                'hostname' => ''
                            ];
                        }
                    }
                }
                @ldap_close($ldap);
            }
        }
    }

    ksort($groups_found);
    uasort($users_found, function($a, $b) {
        return strcasecmp($a['username'], $b['username']);
    });

    $res['ad_groups'] = array_keys($groups_found);
    $res['ad_users'] = array_values($users_found);

    // 3. Current LAN active clients from ARP
    exec("/usr/sbin/arp -an 2>/dev/null", $arp_lines);
    $clients = [];
    foreach ($arp_lines as $l) {
        if (preg_match('/\((\d+\.\d+\.\d+\.\d+)\)\s+at\s+([0-9a-f:]+)/i', $l, $m)) {
            $ip = $m[1];
            $mac = strtolower($m[2]);
            if ($mac === '(incomplete)' || $mac === 'ff:ff:ff:ff:ff:ff') continue;
            $name = 'Misafir Cihaz / Wi-Fi';
            $is_ad = false;
            if (isset($sessions[$ip]) && ($sessions[$ip]['status'] ?? '') === 'authenticated') {
                $name = ($sessions[$ip]['display_name'] ?? $sessions[$ip]['username']) . ' (' . ($sessions[$ip]['hostname'] ?? '') . ')';
                $is_ad = true;
            }
            $clients[] = [
                'ip' => $ip,
                'mac' => $mac,
                'name' => $name,
                'is_ad' => $is_ad
            ];
        }
    }
    $res['lan_clients'] = $clients;

    @file_put_contents($cache_file, json_encode($res, JSON_UNESCAPED_UNICODE));
    return $res;
}

function poseaapp_sync_system($conf = null) {
    if ($conf === null) {
        $conf = poseaapp_get_config();
    }
    global $config;

    $catalog = poseaapp_get_catalog();
    $blocked_domains = [];
    $enabled = !empty($conf['enabled']);

    // Common VPN & Tunnel ports to drop at packet level
    $vpn_ports = '1194,51820,500,4500,1701,1723,2408,41641,7070,5938,9001,9050';

    if ($enabled) {
        // 1. Gather global catalog blocked domains
        foreach ($catalog as $cat_key => $cat) {
            foreach ($cat['apps'] as $aid => $ainfo) {
                $app_setting = $conf['apps'][$aid] ?? ['action' => 'allow'];
                if (($app_setting['action'] ?? 'allow') === 'block') {
                    foreach ($ainfo['domains'] as $d) {
                        $blocked_domains[] = $d;
                    }
                }
            }
        }
        foreach ($conf['custom_apps'] ?? [] as $ca) {
            if (($ca['action'] ?? 'allow') === 'block') {
                $doms = array_map('trim', explode(',', $ca['domains'] ?? ''));
                foreach ($doms as $d) {
                    if (!empty($d)) $blocked_domains[] = $d;
                }
            }
        }
    }

    // Save blocked list to disk for DNS / SNI engine
    $blocked_domains = array_unique($blocked_domains);
    $content = implode("\n", $blocked_domains) . "\n";
    @file_put_contents(POSEAAPP_BLOCKED_FILE, $content);

    // 2. Synchronize pfSense Firewall Rules for VPN Protocols & Specific Client Rules
    if (!isset($config['filter']['rule']) || !is_array($config['filter']['rule'])) {
        $config['filter']['rule'] = [];
    }

    // Clean existing PoseaApp rules
    $cleaned_rules = [];
    foreach ($config['filter']['rule'] as $r) {
        if (is_array($r) && ($r['interface'] ?? '') === 'lan') {
            if (strpos($r['descr'] ?? '', 'PoseaApp Rule:') !== false) {
                continue;
            }
        }
        $cleaned_rules[] = $r;
    }

    if ($enabled) {
        // Read ARP & AD sessions for MAC / User / Group mapping
        $arp_map = [];
        exec("/usr/sbin/arp -an 2>/dev/null", $arp_lines);
        foreach ($arp_lines as $l) {
            if (preg_match('/\((\d+\.\d+\.\d+\.\d+)\)\s+at\s+([0-9a-f:]+)/i', $l, $m)) {
                $arp_map[strtolower($m[2])] = $m[1];
            }
        }

        $ad_sessions = [];
        if (file_exists('/usr/local/pkg/posea_ad_sso.inc')) {
            require_once('/usr/local/pkg/posea_ad_sso.inc');
            if (function_exists('posea_sso_get_sessions')) {
                $ad_sessions = posea_sso_get_sessions();
            }
        }
        if (empty($ad_sessions) && file_exists('/var/etc/poseafilter/ad_users.json')) {
            $ad_sessions = @json_decode(file_get_contents('/var/etc/poseafilter/ad_users.json'), true) ?: [];
        }

        $generated_app_rules = [];

        // Ensure PoseaApp_VPN_Ports port alias exists (exact discrete ports, never a range!)
        if (!isset($config['aliases']['alias']) || !is_array($config['aliases']['alias'])) {
            $config['aliases']['alias'] = [];
        }
        $vpn_alias_idx = null;
        foreach ($config['aliases']['alias'] as $idx => $al) {
            if (($al['name'] ?? '') === 'PoseaApp_VPN_Ports') {
                $vpn_alias_idx = $idx;
                break;
            }
        }
        $vpn_alias_obj = [
            'name' => 'PoseaApp_VPN_Ports',
            'type' => 'port',
            'descr' => 'PoseaApp VPN Tunnel Ports (OpenVPN, WireGuard, IPsec, PPTP, WARP)',
            'address' => '1194 51820 500 4500 1723 2408 41641',
            'detail' => 'OpenVPN||WireGuard||IPsec IKE||IPsec NAT-T||PPTP||Cloudflare WARP||Tailscale'
        ];
        if ($vpn_alias_idx !== null) {
            $config['aliases']['alias'][$vpn_alias_idx] = $vpn_alias_obj;
        } else {
            $config['aliases']['alias'][] = $vpn_alias_obj;
        }

        // Check if global VPN tunnel protocols (OpenVPN & WireGuard) are explicitly blocked
        $global_vpn_blocked = (
            (($conf['apps']['openvpn']['action'] ?? 'allow') === 'block') &&
            (($conf['apps']['wireguard']['action'] ?? 'allow') === 'block')
        );

        // Process granular client rules
        $rule_seq = 1;
        foreach ($conf['client_rules'] ?? [] as $cr) {
            if (empty($cr['enabled'])) continue;

            $t_type = $cr['target_type'] ?? 'ip';
            $t_val = trim($cr['target_val'] ?? '');
            $app_tgt = $cr['app_target'] ?? 'cat_vpn';
            $act = $cr['action'] ?? 'block'; // block or allow
            $descr = $cr['descr'] ?: "{$act} {$app_tgt} for {$t_val}";

            $targets = [];
            if ($t_type === 'ip') {
                $targets[] = ['address' => $t_val];
            } elseif ($t_type === 'subnet') {
                $targets[] = ['network' => $t_val];
            } elseif ($t_type === 'alias') {
                $targets[] = ['address' => $t_val];
            } elseif ($t_type === 'mac') {
                $m_ip = $arp_map[strtolower($t_val)] ?? null;
                if ($m_ip) $targets[] = ['address' => $m_ip];
            } elseif ($t_type === 'user') {
                foreach ($ad_sessions as $u_ip => $u_info) {
                    if (strcasecmp($u_info['username'] ?? '', $t_val) === 0 || strcasecmp($u_info['hostname'] ?? '', $t_val) === 0) {
                        $targets[] = ['address' => $u_ip];
                    }
                }
            } elseif ($t_type === 'group') {
                foreach ($ad_sessions as $u_ip => $u_info) {
                    $g_match = false;
                    if (strcasecmp($u_info['department'] ?? '', $t_val) === 0) $g_match = true;
                    if (!$g_match && !empty($u_info['groups']) && is_array($u_info['groups'])) {
                        foreach ($u_info['groups'] as $grp) {
                            if (strcasecmp($grp, $t_val) === 0) {
                                $g_match = true;
                                break;
                            }
                        }
                    }
                    if ($g_match) $targets[] = ['address' => $u_ip];
                }
            }

            // Determine if rule affects VPN ports
            $vpn_apps = ['cat_vpn', 'cat_privacy', 'openvpn', 'wireguard', 'tor', 'warp', 'nordvpn', 'expressvpn', 'protonvpn', 'surfshark', 'psiphon', 'tailscale', 'claudeflare', 'proton', 'icloud_private_relay', 'privacy'];
            $is_vpn_rule = in_array($app_tgt, $vpn_apps) || strpos($app_tgt, 'vpn') !== false;

            if ($is_vpn_rule) {
                foreach ($targets as $src_def) {
                    $rule_type = ($act === 'allow') ? 'pass' : 'block';
                    $generated_app_rules[] = [
                        'id' => '',
                        'tracker' => (string)(time() + ($rule_seq++)),
                        'type' => $rule_type,
                        'interface' => 'lan',
                        'ipprotocol' => 'inet',
                        'protocol' => 'tcp/udp',
                        'statetype' => 'keep state',
                        'source' => $src_def,
                        'destination' => [
                            'any' => '',
                            'port' => 'PoseaApp_VPN_Ports'
                        ],
                        'descr' => "PoseaApp Rule: " . ($act === 'allow' ? "İzin Ver (Bypass)" : "Engelle") . " VPN [{$t_val}]",
                        'created' => ['time' => (string)time(), 'username' => 'admin (PoseaApp)']
                    ];
                }
            }
        }

        // If global VPN is blocked, add global LAN VPN drop rule at the end of custom app rules
        if ($global_vpn_blocked) {
            $generated_app_rules[] = [
                'id' => '',
                'tracker' => (string)(time() + ($rule_seq++)),
                'type' => 'block',
                'interface' => 'lan',
                'ipprotocol' => 'inet',
                'protocol' => 'tcp/udp',
                'statetype' => 'keep state',
                'source' => ['any' => ''],
                'destination' => [
                    'any' => '',
                    'port' => 'PoseaApp_VPN_Ports'
                ],
                'descr' => "PoseaApp Rule: Global Block VPN Tunnels (OpenVPN/WireGuard/WARP)",
                'created' => ['time' => (string)time(), 'username' => 'admin (PoseaApp)']
            ];
        }

        // Insert rules before Default allow LAN rule
        $final_rules = [];
        $inserted = false;
        foreach ($cleaned_rules as $r) {
            if (is_array($r) && ($r['interface'] ?? '') === 'lan' && strpos($r['descr'] ?? '', 'Default allow LAN') !== false) {
                foreach ($generated_app_rules as $gar) {
                    $final_rules[] = $gar;
                }
                $inserted = true;
            }
            $final_rules[] = $r;
        }
        if (!$inserted) {
            foreach ($generated_app_rules as $gar) {
                $final_rules[] = $gar;
            }
        }
        $config['filter']['rule'] = $final_rules;
    } else {
        $config['filter']['rule'] = $cleaned_rules;
    }

    write_config("PoseaApp: Synchronized application rules and VPN filters");
    filter_configure();

    // Reload filter daemon if running
    if (file_exists('/usr/local/pkg/poseafilter.inc')) {
        require_once('/usr/local/pkg/poseafilter.inc');
        if (function_exists('poseafilter_reload')) {
            poseafilter_reload();
        }
    }
    return count($blocked_domains);
}
