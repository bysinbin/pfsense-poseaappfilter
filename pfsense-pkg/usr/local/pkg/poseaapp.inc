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
        'vpn_tunnels' => [
            'title' => 'VPN, Anonimleşme & Tünelleme (Bypass)',
            'icon' => 'fa-solid fa-shield-halved',
            'apps' => [
                'nordvpn' => [
                    'name' => 'NordVPN',
                    'domains' => ['nordvpn.com', 'nordcdn.com', 'nordaccount.com', 'nordsec.com', 'api.nordvpn.com'],
                    'ports' => '1194,443',
                    'descr' => 'NordVPN tünelleme ve sunucu ağı'
                ],
                'expressvpn' => [
                    'name' => 'ExpressVPN',
                    'domains' => ['expressvpn.com', 'expvpn.com', 'expressvpn.net', 'xv-services.net'],
                    'ports' => '1194,443',
                    'descr' => 'ExpressVPN ticari tünel ağı'
                ],
                'protonvpn' => [
                    'name' => 'ProtonVPN',
                    'domains' => ['protonvpn.com', 'proton.me', 'protonstatus.com', 'protonvpn.net', 'api.protonvpn.ch'],
                    'ports' => '1194,51820',
                    'descr' => 'Proton İsviçre şifreli VPN ağı'
                ],
                'surfshark' => [
                    'name' => 'Surfshark',
                    'domains' => ['surfshark.com', 'surfsharkdns.com', 'shark-services.com'],
                    'ports' => '1194,51820',
                    'descr' => 'Surfshark VPN ve sınırsız cihaz ağı'
                ],
                'openvpn' => [
                    'name' => 'OpenVPN Tünelleri',
                    'domains' => ['openvpn.net', 'openvpn.com', 'my.openvpn.com'],
                    'ports' => '1194,943',
                    'descr' => 'Standart OpenVPN UDP/TCP protokol trafiği'
                ],
                'wireguard' => [
                    'name' => 'WireGuard Tünelleri',
                    'domains' => ['wireguard.com'],
                    'ports' => '51820',
                    'descr' => 'Modern WireGuard UDP şifreli tünelleme'
                ],
                'tor' => [
                    'name' => 'Tor Project (Karanlık Ağ & Köprüler)',
                    'domains' => ['torproject.org', 'onion', 'tor2web.org', 'tor2web.io', 'bridges.torproject.org'],
                    'ports' => '9001,9050,9051',
                    'descr' => 'Tor anonimleştirme ağı ve köprüleri'
                ],
                'warp' => [
                    'name' => 'Cloudflare 1.1.1.1 (WARP)',
                    'domains' => ['cloudflareclient.com', 'warp.plus', 'cloudflare-dns.com'],
                    'ports' => '2408',
                    'descr' => 'Cloudflare mobil ve masaüstü VPN tünelleme'
                ],
                'psiphon' => [
                    'name' => 'Psiphon & UltraSurf',
                    'domains' => ['psiphon.ca', 'psiphon3.com', 'ultrasurf.us', 'ultrasurfing.com'],
                    'ports' => '',
                    'descr' => 'Güvenlik duvarı ve sansür atlatma proxy yazılımları'
                ],
                'tailscale' => [
                    'name' => 'Tailscale Mesh VPN',
                    'domains' => ['tailscale.com', 'tailscale.io', 'controlplane.tailscale.com'],
                    'ports' => '41641',
                    'descr' => 'Zero-trust ağ ve peer-to-peer mesh tünelleme'
                ],
                'windscribe' => [
                    'name' => 'Windscribe',
                    'domains' => ['windscribe.com', 'assets.windscribe.com'],
                    'ports' => '1194,443',
                    'descr' => 'Popüler ücretsiz/ücretli VPN istemcisi'
                ],
                'hotspotshield' => [
                    'name' => 'Hotspot Shield',
                    'domains' => ['hotspotshield.com', 'hsselite.com', 'anchorfree.com'],
                    'ports' => '',
                    'descr' => 'Hydra protokolü kullanan VPN istemcisi'
                ],
                'turbovpn' => [
                    'name' => 'TurboVPN',
                    'domains' => ['turbovpn.com', 'turbospeed.io'],
                    'ports' => '',
                    'descr' => 'Mobil cihazlarda yaygın ücretsiz VPN'
                ],
                'cyberghost' => [
                    'name' => 'CyberGhost',
                    'domains' => ['cyberghostvpn.com', 'cg-dialup.net'],
                    'ports' => '1194',
                    'descr' => 'CyberGhost VPN sunucu ve tünel ağı'
                ]
            ]
        ],
        'remote_access' => [
            'title' => 'Uzak Masaüstü & Shadow IT (Remote Control)',
            'icon' => 'fa-solid fa-display',
            'apps' => [
                'anydesk' => [
                    'name' => 'AnyDesk',
                    'domains' => ['anydesk.com', 'net.anydesk.com', 'boot.net.anydesk.com'],
                    'ports' => '7070',
                    'descr' => 'AnyDesk uzaktan kontrol ve dosya transferi'
                ],
                'teamviewer' => [
                    'name' => 'TeamViewer',
                    'domains' => ['teamviewer.com', 'teamviewer.net', 'dyngate.com'],
                    'ports' => '5938',
                    'descr' => 'TeamViewer uzaktan yönetim ve masaüstü paylaşımı'
                ],
                'rustdesk' => [
                    'name' => 'RustDesk',
                    'domains' => ['rustdesk.com'],
                    'ports' => '21115,21116,21117,21118,21119',
                    'descr' => 'Açık kaynak kendi kendine barındırılan uzaktan masaüstü'
                ],
                'chromeremotedesktop' => [
                    'name' => 'Chrome Remote Desktop',
                    'domains' => ['remotedesktop.google.com', 'chromeremotedesktop.google.com'],
                    'ports' => '',
                    'descr' => 'Google Chrome tarayıcı tabanlı uzaktan erişim'
                ]
            ]
        ],
        'streaming' => [
            'title' => 'Video & Müzik (Streaming)',
            'icon' => 'fa-solid fa-play',
            'apps' => [
                'youtube' => [
                    'name' => 'YouTube',
                    'domains' => ['youtube.com', 'youtu.be', 'googlevideo.com', 'ytimg.com'],
                    'ports' => '',
                    'descr' => 'Google video akış platformu'
                ],
                'netflix' => [
                    'name' => 'Netflix',
                    'domains' => ['netflix.com', 'nflxvideo.net', 'nflximg.net', 'nflxext.com'],
                    'ports' => '',
                    'descr' => 'Film ve dizi akış servisi'
                ],
                'tiktok' => [
                    'name' => 'TikTok',
                    'domains' => ['tiktok.com', 'tiktokv.com', 'byteoversea.com', 'ibytedtos.com', 'musically.com', 'musical.ly'],
                    'ports' => '',
                    'descr' => 'ByteDance kısa video platformu'
                ],
                'twitch' => [
                    'name' => 'Twitch',
                    'domains' => ['twitch.tv', 'ttvnw.net', 'jtvnw.net'],
                    'ports' => '',
                    'descr' => 'Canlı yayın ve oyun akış servisi'
                ],
                'spotify' => [
                    'name' => 'Spotify',
                    'domains' => ['spotify.com', 'scdn.co', 'spoti.fi'],
                    'ports' => '',
                    'descr' => 'Müzik ve podcast akış servisi'
                ]
            ]
        ],
        'gaming' => [
            'title' => 'Çevrimiçi Oyunlar & Platformlar',
            'icon' => 'fa-solid fa-gamepad',
            'apps' => [
                'roblox' => [
                    'name' => 'Roblox',
                    'domains' => ['roblox.com', 'rbxcdn.com'],
                    'ports' => '',
                    'descr' => 'Çok oyunculu oyun ve sanal evren'
                ],
                'steam' => [
                    'name' => 'Steam',
                    'domains' => ['steampowered.com', 'steamcommunity.com', 'steamstatic.com'],
                    'ports' => '',
                    'descr' => 'Valve oyun mağazası ve ağı'
                ],
                'epicgames' => [
                    'name' => 'Epic Games (Fortnite)',
                    'domains' => ['epicgames.com', 'unrealengine.com'],
                    'ports' => '',
                    'descr' => 'Epic Games başlatıcısı ve mağazası'
                ],
                'riotgames' => [
                    'name' => 'Riot Games (Valorant / LoL)',
                    'domains' => ['riotgames.com', 'leagueoflegends.com', 'playvalorant.com'],
                    'ports' => '',
                    'descr' => 'Riot Games oyun sunucuları'
                ],
                'discord' => [
                    'name' => 'Discord',
                    'domains' => ['discord.com', 'discord.gg', 'discordapp.com', 'discordapp.net'],
                    'ports' => '',
                    'descr' => 'Sesli ve yazılı oyuncu topluluk ağı'
                ]
            ]
        ],
        'p2p_bypass' => [
            'title' => 'P2P & Dosya Paylaşımı (Torrent)',
            'icon' => 'fa-solid fa-download',
            'apps' => [
                'bittorrent' => [
                    'name' => 'BitTorrent / uTorrent / P2P',
                    'domains' => ['bittorrent.com', 'utorrent.com', 'torrent.com', 'thepiratebay.org', '1337x.to'],
                    'ports' => '6881,6882,6883,6884,6885,6886,6887,6888,6889',
                    'descr' => 'P2P dosya paylaşım protokolleri ve trackerlar'
                ]
            ]
        ],
        'social' => [
            'title' => 'Sosyal Medya Ağları',
            'icon' => 'fa-solid fa-share-nodes',
            'apps' => [
                'instagram' => [
                    'name' => 'Instagram',
                    'domains' => ['instagram.com', 'cdninstagram.com'],
                    'ports' => '',
                    'descr' => 'Fotoğraf ve video paylaşım ağı'
                ],
                'facebook' => [
                    'name' => 'Facebook',
                    'domains' => ['facebook.com', 'fbcdn.net', 'facebook.net'],
                    'ports' => '',
                    'descr' => 'Meta sosyal paylaşım ağı'
                ],
                'twitter_x' => [
                    'name' => 'X (Twitter)',
                    'domains' => ['x.com', 'twitter.com', 'twimg.com', 't.co'],
                    'ports' => '',
                    'descr' => 'Mikroblog ve haber platformu'
                ]
            ]
        ],
        'messaging' => [
            'title' => 'Anlık Mesajlaşma & Arama',
            'icon' => 'fa-solid fa-comments',
            'apps' => [
                'whatsapp' => [
                    'name' => 'WhatsApp Web & Masaüstü',
                    'domains' => ['whatsapp.com', 'whatsapp.net'],
                    'ports' => '',
                    'descr' => 'Meta anlık mesajlaşma ve dosya transferi'
                ],
                'telegram' => [
                    'name' => 'Telegram',
                    'domains' => ['telegram.org', 't.me', 'telegram.me'],
                    'ports' => '',
                    'descr' => 'Şifreli mesajlaşma ve kanal ağı'
                ]
            ]
        ],
        'crypto' => [
            'title' => 'Kripto Para Borsaları & Madencilik',
            'icon' => 'fa-solid fa-coins',
            'apps' => [
                'binance' => [
                    'name' => 'Binance',
                    'domains' => ['binance.com', 'binance.me', 'binance.vision'],
                    'ports' => '',
                    'descr' => 'Kripto para alım satım platformu'
                ],
                'btcturk' => [
                    'name' => 'BtcTurk & Paribu',
                    'domains' => ['btcturk.com', 'paribu.com'],
                    'ports' => '',
                    'descr' => 'Yerel kripto para borsaları'
                ]
            ]
        ]
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
            $is_vpn_rule = ($app_tgt === 'cat_vpn' || strpos($app_tgt, 'vpn') !== false || in_array($app_tgt, ['openvpn', 'wireguard', 'tor', 'warp', 'nordvpn', 'expressvpn']));

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
