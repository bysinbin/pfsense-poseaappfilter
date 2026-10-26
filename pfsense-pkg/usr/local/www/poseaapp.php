<?php
/*
 * poseaapp.php - PoseaApp Layer 7 Application Control, VPN & Protocol Filtering
 * Part of Posea Security Suite
 */

require_once("guiconfig.inc");
require_once("/usr/local/pkg/poseafilter.inc");
require_once("/usr/local/pkg/poseaapp.inc");

$conf = poseaapp_get_config();
$catalog = poseaapp_get_catalog();
$savemsg = "";

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $act = $_POST['act'] ?? '';
    
    if ($act === 'save_apps') {
        $conf['enabled'] = isset($_POST['enabled']);
        $conf['log_blocks'] = isset($_POST['log_blocks']);
        
        $submitted_apps = $_POST['apps'] ?? [];
        foreach ($catalog as $cat) {
            foreach ($cat['apps'] as $aid => $ainfo) {
                $action = $submitted_apps[$aid] ?? 'allow';
                $conf['apps'][$aid] = ['action' => $action];
            }
        }
        
        poseaapp_save_config($conf);
        $blocked_count = poseaapp_sync_system($conf);
        $savemsg = "PoseaApp uygulama kuralları kaydedildi ve uygulandı. " . (!empty($conf['enabled']) ? "({$blocked_count} alan adı ve ilgili VPN portları filtreleniyor)" : "(Uygulama filtresi pasife alındı)");

    } elseif ($act === 'save_client_rule') {
        $r_id = trim($_POST['rule_id'] ?? '');
        $t_type = trim($_POST['target_type'] ?? 'group');
        $t_val = trim($_POST['target_val'] ?? '');
        $app_tgt = trim($_POST['app_target'] ?? 'cat_vpn');
        $action = trim($_POST['rule_action'] ?? 'block');
        $descr = trim($_POST['rule_descr'] ?? '');

        if ($t_val) {
            $found = false;
            if ($r_id) {
                foreach ($conf['client_rules'] as &$r) {
                    if (($r['id'] ?? '') === $r_id) {
                        $r['target_type'] = $t_type;
                        $r['target_val'] = $t_val;
                        $r['app_target'] = $app_tgt;
                        $r['action'] = $action;
                        $r['descr'] = $descr;
                        $r['enabled'] = true;
                        $found = true;
                        break;
                    }
                }
                unset($r);
            }
            if (!$found) {
                $r_id = 'apprule_' . substr(md5(uniqid()), 0, 6);
                $conf['client_rules'][] = [
                    'id' => $r_id,
                    'target_type' => $t_type,
                    'target_val' => $t_val,
                    'app_target' => $app_tgt,
                    'action' => $action,
                    'descr' => $descr,
                    'enabled' => true
                ];
                $savemsg = "Özel uygulama kuralı ('{$t_val}') başarıyla eklendi ve kernel seviyesinde uygulandı.";
            } else {
                $savemsg = "Uygulama kuralı ('{$t_val}') güncellendi.";
            }
            poseaapp_save_config($conf);
            poseaapp_sync_system($conf);
        }

    } elseif ($act === 'del_client_rule') {
        $r_id = trim($_POST['rule_id'] ?? '');
        $conf['client_rules'] = array_values(array_filter($conf['client_rules'] ?? [], function($r) use ($r_id) {
            return ($r['id'] ?? '') !== $r_id;
        }));
        poseaapp_save_config($conf);
        poseaapp_sync_system($conf);
        $savemsg = "Uygulama kuralı silindi.";

    } elseif ($act === 'add_custom_app') {
        $c_name = trim($_POST['custom_name'] ?? '');
        $c_cat = trim($_POST['custom_cat'] ?? 'Genel');
        $c_doms = trim($_POST['custom_doms'] ?? '');
        $c_act = $_POST['custom_action'] ?? 'block';
        
        if (!empty($c_name) && !empty($c_doms)) {
            $conf['custom_apps'][] = [
                'id' => 'cust_' . substr(md5(uniqid()), 0, 6),
                'name' => $c_name,
                'category' => $c_cat,
                'domains' => $c_doms,
                'action' => $c_act
            ];
            poseaapp_save_config($conf);
            poseaapp_sync_system($conf);
            $savemsg = "Özel uygulama imzası '{$c_name}' başarıyla eklendi.";
        }

    } elseif ($act === 'del_custom_app') {
        $cid = $_POST['custom_id'] ?? '';
        $conf['custom_apps'] = array_values(array_filter($conf['custom_apps'] ?? [], function($a) use ($cid) {
            return ($a['id'] ?? '') !== $cid;
        }));
        poseaapp_save_config($conf);
        poseaapp_sync_system($conf);
        $savemsg = "Özel uygulama silindi.";
    }
}

// Fetch AD & pfSense Aliases Metadata
$meta = poseaapp_get_ad_metadata();
$meta_aliases = $meta['aliases'] ?? [];
$meta_groups = $meta['ad_groups'] ?? [];
$meta_users = $meta['ad_users'] ?? [];
$meta_clients = $meta['lan_clients'] ?? [];

$pgtitle = array(gettext("Services"), gettext("PoseaApp (L7 Uygulama & VPN Filtresi)"));
include("head.inc");

$tab_array = array();
$tab_array[] = array(gettext("PoseaBand (QoS)"), false, "/poseaband.php");
$tab_array[] = array(gettext("PoseaInsight"), false, "/poseainsight.php");
$tab_array[] = array(gettext("PoseaApp"), true, "/poseaapp.php");
$tab_array[] = array(gettext("PoseaAccess"), false, "/poseaaccess.php");
display_top_tabs($tab_array);

if ($savemsg) {
    print_info_box($savemsg, 'success');
}
?>

<div class="panel panel-default">
    <div class="panel-heading">
        <h2 class="panel-title">
            <i class="fa-solid fa-shapes"></i> PoseaApp &bull; Katman 7 (L7) Uygulama Denetimi, VPN &amp; Protokol Filtresi
        </h2>
    </div>
    <div class="panel-body">
        <p class="text-muted">
            PoseaApp; VPN tünelleri (NordVPN, Tor, OpenVPN, WireGuard), uzaktan kontrol yazılımları (AnyDesk, TeamViewer) ve popüler uygulamaları (YouTube, Netflix, TikTok, Roblox) tek tıkla veya <strong>Active Directory grup ve kullanıcı bazlı</strong> kurallarla yönetmenizi sağlar.
        </p>

        <!-- FORM 1: Global Settings and Enable Status -->
        <form method="post" id="globalAppForm" novalidate>
            <input type="hidden" name="act" value="save_apps">

            <div class="alert <?=!empty($conf['enabled']) ? 'alert-success' : 'alert-warning'?>" style="display:flex; align-items:center; justify-content:space-between; padding:12px 18px; border-radius:6px;">
                <div style="font-size:14px;">
                    <strong><i class="fa-solid fa-layer-group"></i> PoseaApp Servis Durumu:</strong>
                    <?php if (!empty($conf['enabled'])): ?>
                        <span class="badge" style="background-color:#16a34a; font-size:13px; margin-left:8px; padding:6px 12px;">AKTİF (Uygulama &amp; VPN Filtresi Çalışıyor)</span>
                    <?php else: ?>
                        <span class="badge" style="background-color:#64748b; font-size:13px; margin-left:8px; padding:6px 12px;">PASİF (Filtre Devre Dışı)</span>
                    <?php endif; ?>
                </div>
                <div class="checkbox" style="margin:0;">
                    <label style="font-weight:700; color:#0f172a; cursor:pointer; font-size:13px;">
                        <input type="checkbox" name="enabled" value="1" <?=!empty($conf['enabled']) ? 'checked' : ''?> onchange="this.form.submit();" style="transform:scale(1.2); margin-right:8px;">
                        PoseaApp Servisini Etkinleştir
                    </label>
                </div>
            </div>

            <!-- SECTION 1: Granular Rule-Based Application & VPN Control -->
            <div class="panel panel-warning" style="margin-top:20px; border-radius:8px; box-shadow:0 1px 3px rgba(0,0,0,0.05);">
                <div class="panel-heading" style="display:flex; justify-content:space-between; align-items:center; background:#0f172a; color:#fff;">
                    <strong style="font-size:14px;"><i class="fa-solid fa-users-gear text-warning"></i> Özel Kural Tabanlı Uygulama &amp; VPN Denetimi (AD / İstemci Kuralları)</strong>
                    <button type="button" class="btn btn-xs btn-success" onclick="openNewRuleForm();"><i class="fa-solid fa-plus"></i> Yeni Uygulama Kuralı Tanımla</button>
                </div>
                <div class="panel-body">
                    <p class="text-muted">
                        Active Directory gruplarına (örn: <code>Domain Users</code>, <code>Muhasebe</code>), tekil kullanıcılara, pfSense Alias'larına veya IP alt ağlarına (Subnet) özel uygulama ve VPN izin/engelleme kuralları atayabilirsiniz. Tanımlanan kurallar genel katalog ayarlarının önüne geçer.
                    </p>

                    <!-- Dark Add/Edit Rule Collapse Panel with Isolated Form -->
                    <div id="newRulePanel" class="collapse" style="background:#1e293b; color:#f1f5f9; padding:20px; border-radius:8px; margin-bottom:20px; border:1px solid #10b981; box-shadow:0 4px 6px -1px rgba(0,0,0,0.3);">
                        <form method="post" id="ruleForm" novalidate onsubmit="return validateRuleForm();">
                            <input type="hidden" name="act" value="save_client_rule">
                            <input type="hidden" name="rule_id" id="rule_id_input" value="">
                            <input type="hidden" name="target_val" id="rule_target_val" value="">

                            <div style="display:flex; justify-content:space-between; align-items:center; margin-bottom:14px; border-bottom:1px solid #334155; padding-bottom:8px;">
                                <h4 style="color:#34d399; margin:0; font-weight:700;">
                                    <i class="fa-solid fa-shield-virus"></i> <span id="ruleFormTitle">Yeni Uygulama &amp; VPN Kuralı Tanımla</span>
                                </h4>
                                <button type="button" class="btn btn-xs btn-default" onclick="$('#newRulePanel').collapse('hide');" style="color:#94a3b8;"><i class="fa fa-times"></i> Kapat</button>
                            </div>

                            <div class="row">
                                <div class="col-md-3">
                                    <div class="form-group" style="margin-bottom:12px;">
                                        <label class="control-label" style="color:#cbd5e1; font-weight:600;">Hedef Tipi:</label>
                                        <select name="target_type" id="rule_target_type" class="form-control" style="background:#0f172a; color:#fff; border:1px solid #475569;" onchange="onTargetTypeChange();">
                                            <option value="group" selected>👥 Active Directory Grubu / Departman</option>
                                            <option value="user">👤 Active Directory Kullanıcısı / PC</option>
                                            <option value="alias">📋 pfSense Alias (IP / Host / Ağ Listesi)</option>
                                            <option value="subnet">🌐 IP Alt Ağı (Subnet / CIDR)</option>
                                            <option value="ip">💻 Tekil İstemci IP Adresi</option>
                                            <option value="mac">🏷️ Cihaz MAC Adresi</option>
                                        </select>
                                    </div>
                                </div>

                                <div class="col-md-3">
                                    <div class="form-group" style="margin-bottom:12px;">
                                        <div style="display:flex; justify-content:space-between; align-items:center; margin-bottom:4px;">
                                            <label class="control-label" style="color:#cbd5e1; font-weight:600; margin:0;">Hedef Değer:</label>
                                            <a href="javascript:void(0)" onclick="toggleManualTargetInput();" style="color:#38bdf8; font-size:11px; text-decoration:none;"><i class="fa fa-keyboard-o"></i> <span id="manualToggleLabel">Elle Yaz</span></a>
                                        </div>

                                        <!-- AD Group Select -->
                                        <div id="wrap_group" class="target-field-wrap">
                                            <select id="sel_group" class="form-control" style="background:#0f172a; color:#fff; border:1px solid #475569;" onchange="syncTargetVal(this.value)">
                                                <option value="">-- AD Grubu / Departman Seçiniz (<?=count($meta_groups)?> Grup) --</option>
                                                <?php foreach ($meta_groups as $grp): ?>
                                                    <option value="<?=htmlspecialchars($grp)?>"><?=htmlspecialchars($grp)?></option>
                                                <?php endforeach; ?>
                                            </select>
                                        </div>

                                        <!-- AD User Select -->
                                        <div id="wrap_user" class="target-field-wrap" style="display:none;">
                                            <select id="sel_user" class="form-control" style="background:#0f172a; color:#fff; border:1px solid #475569;" onchange="syncTargetVal(this.value)">
                                                <option value="">-- AD Kullanıcısı / PC Seçiniz (<?=count($meta_users)?> Kullanıcı) --</option>
                                                <?php foreach ($meta_users as $usr): ?>
                                                    <option value="<?=htmlspecialchars($usr['username'])?>"><?=htmlspecialchars($usr['username'])?> <?=!empty($usr['display_name']) && $usr['display_name'] !== $usr['username'] ? '('.htmlspecialchars($usr['display_name']).')' : ''?></option>
                                                <?php endforeach; ?>
                                            </select>
                                        </div>

                                        <!-- Alias Select -->
                                        <div id="wrap_alias" class="target-field-wrap" style="display:none;">
                                            <select id="sel_alias" class="form-control" style="background:#0f172a; color:#fff; border:1px solid #475569;" onchange="syncTargetVal(this.value)">
                                                <option value="">-- pfSense Alias Seçiniz (<?=count($meta_aliases)?> Alias) --</option>
                                                <?php foreach ($meta_aliases as $als): ?>
                                                    <option value="<?=htmlspecialchars($als['name'])?>"><?=htmlspecialchars($als['name'])?> (<?=htmlspecialchars($als['type'])?>: <?=htmlspecialchars(mb_substr($als['address'], 0, 30))?>)</option>
                                                <?php endforeach; ?>
                                            </select>
                                        </div>

                                        <!-- Subnet Input -->
                                        <div id="wrap_subnet" class="target-field-wrap" style="display:none;">
                                            <input type="text" id="sel_subnet" class="form-control" placeholder="Örn: 192.168.2.0/24" style="background:#0f172a; color:#fff; border:1px solid #475569;" oninput="syncTargetVal(this.value)">
                                        </div>

                                        <!-- IP Input with LAN Clients Datalist -->
                                        <div id="wrap_ip" class="target-field-wrap" style="display:none;">
                                            <input type="text" id="sel_ip" class="form-control" list="dl_lan_ips" placeholder="Örn: 192.168.2.226" style="background:#0f172a; color:#fff; border:1px solid #475569;" oninput="syncTargetVal(this.value)">
                                            <datalist id="dl_lan_ips">
                                                <?php foreach ($meta_clients as $cli): ?>
                                                    <option value="<?=htmlspecialchars($cli['ip'])?>"><?=htmlspecialchars($cli['name'])?> (<?=htmlspecialchars($cli['mac'])?>)</option>
                                                <?php endforeach; ?>
                                            </datalist>
                                        </div>

                                        <!-- MAC Input with LAN MACs Datalist -->
                                        <div id="wrap_mac" class="target-field-wrap" style="display:none;">
                                            <input type="text" id="sel_mac" class="form-control" list="dl_lan_macs" placeholder="Örn: a6:73:5a:e1:34:c1" style="background:#0f172a; color:#fff; border:1px solid #475569;" oninput="syncTargetVal(this.value)">
                                            <datalist id="dl_lan_macs">
                                                <?php foreach ($meta_clients as $cli): ?>
                                                    <option value="<?=htmlspecialchars($cli['mac'])?>"><?=htmlspecialchars($cli['ip'])?> - <?=htmlspecialchars($cli['name'])?></option>
                                                <?php endforeach; ?>
                                            </datalist>
                                        </div>

                                        <!-- Fallback Manual Input -->
                                        <div id="wrap_manual" class="target-field-wrap" style="display:none;">
                                            <input type="text" id="sel_manual" class="form-control" placeholder="Özel Değer Yazınız..." style="background:#0f172a; color:#fff; border:1px solid #475569;" oninput="syncTargetVal(this.value)">
                                        </div>
                                    </div>
                                </div>

                                <div class="col-md-3">
                                    <div class="form-group" style="margin-bottom:12px;">
                                        <label class="control-label" style="color:#cbd5e1; font-weight:600;">Denetlenecek Uygulama / Kategori:</label>
                                        <select name="app_target" id="rule_app_target" class="form-control" style="background:#0f172a; color:#fff; border:1px solid #475569;">
                                            <optgroup label="── GENEL KATEGORİLER (TÜM GRUP) ──">
                                                <?php foreach ($catalog as $cat_key => $cat_info): ?>
                                                    <option value="cat_<?=$cat_key?>"><?=htmlspecialchars($cat_info['title'])?> (Tümü)</option>
                                                <?php endforeach; ?>
                                            </optgroup>
                                            <?php foreach ($catalog as $cat_key => $cat_info): ?>
                                                <optgroup label="── <?=htmlspecialchars(mb_strtoupper($cat_info['title'], 'UTF-8'))?> ──">
                                                    <?php foreach ($cat_info['apps'] as $aid => $ainfo): ?>
                                                        <option value="<?=$aid?>"><?=htmlspecialchars($ainfo['name'])?></option>
                                                    <?php endforeach; ?>
                                                </optgroup>
                                            <?php endforeach; ?>
                                        </select>
                                    </div>
                                </div>

                                <div class="col-md-3">
                                    <div class="row">
                                        <div class="col-xs-5">
                                            <div class="form-group" style="margin-bottom:12px;">
                                                <label class="control-label" style="color:#cbd5e1; font-weight:600;">Eylem:</label>
                                                <select name="rule_action" id="rule_action" class="form-control" style="background:#0f172a; color:#fff; border:1px solid #475569;">
                                                    <option value="block" selected>🚫 Engelle</option>
                                                    <option value="allow">✅ İzin Ver</option>
                                                </select>
                                            </div>
                                        </div>
                                        <div class="col-xs-7">
                                            <div class="form-group" style="margin-bottom:12px;">
                                                <label class="control-label" style="color:#cbd5e1; font-weight:600;">Açıklama / Not:</label>
                                                <input type="text" name="rule_descr" id="rule_descr" class="form-control" placeholder="Örn: Muhasebe VPN Yasağı" style="background:#0f172a; color:#fff; border:1px solid #475569;">
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            </div>

                            <div class="text-right" style="border-top:1px solid #334155; padding-top:12px; margin-top:10px;">
                                <button type="button" class="btn btn-default btn-sm" onclick="$('#newRulePanel').collapse('hide');" style="margin-right:6px;"><i class="fa fa-times"></i> Vazgeç</button>
                                <button type="submit" id="ruleSubmitBtn" class="btn btn-success btn-sm"><i class="fa-solid fa-check"></i> Kuralı Kaydet &amp; Uygula</button>
                            </div>
                        </form>
                    </div>

                    <!-- Client Rules Table -->
                    <table class="table table-hover table-striped">
                        <thead>
                            <tr style="background:#090d16; color:#f8fafc;">
                                <th>Hedef Değer</th>
                                <th>Hedef Tipi</th>
                                <th>Denetlenen Uygulama / Kategori</th>
                                <th>Eylem</th>
                                <th>Açıklama</th>
                                <th style="text-align:right; width:130px;">İşlemler</th>
                            </tr>
                        </thead>
                        <tbody>
                            <?php if (empty($conf['client_rules'])): ?>
                                <tr>
                                    <td colspan="6" class="text-center text-muted" style="padding:20px;">
                                        <i class="fa-solid fa-circle-info"></i> Henüz özel bir uygulama veya VPN kuralı tanımlanmadı. Ağınız aşağıdaki genel katalog politikaları ile yönetiliyor.
                                    </td>
                                </tr>
                            <?php else: ?>
                                <?php foreach ($conf['client_rules'] as $cr): 
                                    $type_labels = [
                                        'group' => '<span class="label label-success"><i class="fa-solid fa-users"></i> AD Grubu</span>',
                                        'user' => '<span class="label label-warning"><i class="fa-solid fa-user"></i> AD Kullanıcı</span>',
                                        'alias' => '<span class="label label-primary"><i class="fa-solid fa-list"></i> pfSense Alias</span>',
                                        'subnet' => '<span class="label label-info"><i class="fa-solid fa-sitemap"></i> Subnet</span>',
                                        'ip' => '<span class="label label-default"><i class="fa-solid fa-desktop"></i> IP Adresi</span>',
                                        'mac' => '<span class="label label-info"><i class="fa-solid fa-barcode"></i> MAC Adresi</span>'
                                    ];
                                    $tgt = $cr['app_target'] ?? '';
                                    $app_display = '';
                                    if (strpos($tgt, 'cat_') === 0) {
                                        $ckey = substr($tgt, 4);
                                        if (isset($catalog[$ckey])) {
                                            $app_display = '<span class="label label-danger" style="font-size:11px;"><i class="' . htmlspecialchars($catalog[$ckey]['icon']) . '"></i> Tüm ' . htmlspecialchars($catalog[$ckey]['title']) . '</span>';
                                        } else {
                                            $app_display = '<span class="label label-danger" style="font-size:11px;"><i class="fa-solid fa-layer-group"></i> ' . htmlspecialchars($tgt) . '</span>';
                                        }
                                    } else {
                                        $found_app = null;
                                        $found_icon = 'fa-solid fa-cube';
                                        foreach ($catalog as $ck => $ci) {
                                            if (isset($ci['apps'][$tgt])) {
                                                $found_app = $ci['apps'][$tgt]['name'];
                                                $found_icon = $ci['icon'];
                                                break;
                                            }
                                        }
                                        if ($found_app) {
                                            $app_display = '<span class="label label-default" style="background:#334155; font-size:11px;"><i class="' . htmlspecialchars($found_icon) . '"></i> ' . htmlspecialchars($found_app) . '</span>';
                                        } else {
                                            $app_display = '<span class="label label-default" style="font-size:11px;">' . htmlspecialchars(ucfirst($tgt)) . '</span>';
                                        }
                                    }
                                ?>
                                <tr>
                                    <td><strong style="color:#f8fafc; font-size:13px;"><?=htmlspecialchars($cr['target_val'])?></strong></td>
                                    <td><?=$type_labels[$cr['target_type']] ?? '<span class="label label-default">'.htmlspecialchars($cr['target_type']).'</span>'?></td>
                                    <td><?=$app_display?></td>
                                    <td>
                                        <?php if (($cr['action'] ?? 'block') === 'block'): ?>
                                            <span class="badge" style="background:#ef4444; font-size:11px;"><i class="fa-solid fa-ban"></i> Engellendi</span>
                                        <?php else: ?>
                                            <span class="badge" style="background:#10b981; font-size:11px;"><i class="fa-solid fa-check"></i> İzin Verildi</span>
                                        <?php endif; ?>
                                    </td>
                                    <td><small class="text-muted"><?=htmlspecialchars($cr['descr'])?></small></td>
                                    <td style="text-align:right; white-space:nowrap;">
                                        <button type="button" class="btn btn-xs btn-primary" onclick='editClientRule(<?=json_encode($cr, JSON_UNESCAPED_UNICODE)?>)' title="Kuralı Düzenle">
                                            <i class="fa-solid fa-pencil"></i> Düzenle
                                        </button>
                                        <button type="button" onclick="deleteClientRule('<?=htmlspecialchars($cr['id'], ENT_QUOTES, 'UTF-8')?>', '<?=htmlspecialchars($cr['target_val'], ENT_QUOTES, 'UTF-8')?>')" class="btn btn-xs btn-danger" title="Kuralı Sil" style="margin-left:4px;">
                                            <i class="fa-solid fa-trash"></i>
                                        </button>
                                    </td>
                                </tr>
                                <?php endforeach; ?>
                            <?php endif; ?>
                        </tbody>
                    </table>
                </div>
            </div>

            <!-- SECTION 2: Global Application Catalog Cards -->
            <div style="display:flex; justify-content:space-between; align-items:center; margin:25px 0 15px 0;">
                <h3 style="margin:0; font-weight:700; color:#f8fafc;">
                    <i class="fa-solid fa-boxes-stacked text-primary"></i> Genel Uygulama Kataloğu
                </h3>
                <div style="display:flex; gap:8px;">
                    <button type="button" class="btn btn-xs btn-default" onclick="setAllApps('allow');"><i class="fa-solid fa-check-double text-success"></i> Tümüne İzin Ver</button>
                    <button type="button" class="btn btn-xs btn-warning" onclick="blockAllVpns();"><i class="fa-solid fa-shield-halved"></i> Tüm VPN'leri Engelle</button>
                    <button type="button" class="btn btn-xs btn-default" onclick="setRecommended();"><i class="fa-solid fa-shield text-primary"></i> Standart Güvenlik (VPN/P2P/Tor/TikTok/Oyun)</button>
                    <button type="button" class="btn btn-xs btn-danger" onclick="setAllApps('block');"><i class="fa-solid fa-ban"></i> Tümünü Engelle</button>
                </div>
            </div>

            <div class="row">
                <?php foreach ($catalog as $cat_key => $cat_info): ?>
                <div class="col-md-6" style="margin-bottom:20px;">
                    <div class="panel panel-info" style="border-radius:8px; box-shadow:0 1px 3px rgba(0,0,0,0.05);">
                        <div class="panel-heading" style="background:#1e293b; color:#fff; font-weight:bold; display:flex; justify-content:space-between; align-items:center;">
                            <span><i class="<?=$cat_info['icon']?> text-info"></i> <?=htmlspecialchars($cat_info['title'])?></span>
                            <div style="display:flex; align-items:center; gap:6px;">
                                <span class="badge" style="background:#334155; font-size:11px;"><?=count($cat_info['apps'])?> Uygulama</span>
                                <button type="button" class="btn btn-xs btn-default" style="padding:1px 6px; font-size:10px; background:#0f172a; color:#22c55e; border:1px solid #334155;" onclick="setCategoryApps('<?=$cat_key?>', 'allow')" title="Bu kategorideki tüm uygulamalara izin ver"><i class="fa-solid fa-check"></i> İzin Ver</button>
                                <button type="button" class="btn btn-xs btn-default" style="padding:1px 6px; font-size:10px; background:#0f172a; color:#ef4444; border:1px solid #334155;" onclick="setCategoryApps('<?=$cat_key?>', 'block')" title="Bu kategorideki tüm uygulamaları engelle"><i class="fa-solid fa-ban"></i> Engelle</button>
                            </div>
                        </div>
                        <div class="panel-body" style="padding:10px 15px;">
                            <table class="table table-hover table-condensed" style="margin-bottom:0;">
                                <tbody>
                                    <?php foreach ($cat_info['apps'] as $aid => $ainfo): 
                                        $cur_act = $conf['apps'][$aid]['action'] ?? 'allow';
                                    ?>
                                    <tr>
                                        <td style="vertical-align:middle; width:45%;">
                                            <strong><?=htmlspecialchars($ainfo['name'])?></strong><br>
                                            <small class="text-muted"><?=htmlspecialchars($ainfo['descr'])?></small>
                                            <?php if (!empty($ainfo['ports'])): ?>
                                                <br><small class="text-info"><i class="fa-solid fa-network-wired"></i> Port: <?=htmlspecialchars($ainfo['ports'])?></small>
                                            <?php endif; ?>
                                        </td>
                                        <td style="vertical-align:middle; width:35%;">
                                            <small class="text-muted font-monospace"><?=htmlspecialchars(implode(', ', array_slice($ainfo['domains'], 0, 2)))?>...</small>
                                        </td>
                                        <td style="vertical-align:middle; width:20%; text-align:right;">
                                            <select name="apps[<?=$aid?>]" class="form-control input-sm app-selector" data-category="<?=$cat_key?>" style="<?=$cur_act === 'block' ? 'background:#fee2e2; color:#b91c1c; font-weight:bold;' : 'background:#dcfce7; color:#15803d; font-weight:bold;'?>" onchange="styleSelector(this);">
                                                <option value="allow" <?=$cur_act === 'allow' ? 'selected' : ''?>>İzin Ver</option>
                                                <option value="block" <?=$cur_act === 'block' ? 'selected' : ''?>>Engelle</option>
                                            </select>
                                        </td>
                                    </tr>
                                    <?php endforeach; ?>
                                </tbody>
                            </table>
                        </div>
                    </div>
                </div>
                <?php endforeach; ?>
            </div>

            <div class="text-right" style="margin-top:20px;">
                <button type="submit" class="btn btn-primary btn-lg"><i class="fa-solid fa-floppy-disk"></i> Uygulama Kataloğunu Kaydet &amp; Uygula</button>
            </div>
        </form> <!-- CLOSE globalAppForm -->

        <hr style="margin:25px 0; border-color:#334155;">

        <!-- SECTION 3: Custom App Signatures with Isolated Form -->
        <div class="panel panel-default" style="margin-top:10px;">
            <div class="panel-heading" style="display:flex; justify-content:space-between; align-items:center;">
                <strong><i class="fa-solid fa-code"></i> Özel Uygulama İmzaları</strong>
                <button type="button" class="btn btn-xs btn-success" data-toggle="collapse" data-target="#customAppPanel"><i class="fa-solid fa-plus"></i> Özel Uygulama Tanımla</button>
            </div>
            <div class="panel-body">
                <div id="customAppPanel" class="collapse" style="background:#1e293b; color:#f1f5f9; padding:18px; border-radius:8px; margin-bottom:15px; border:1px solid #3b82f6;">
                    <form method="post" id="customAppForm" novalidate onsubmit="return validateCustomAppForm();">
                        <input type="hidden" name="act" value="add_custom_app">

                        <h4 style="color:#60a5fa; margin-top:0; font-weight:700;"><i class="fa-solid fa-plus-circle"></i> Yeni Uygulama İmzası Tanımla</h4>
                        <div class="form-group" style="margin-bottom:12px;">
                            <label class="col-sm-3 control-label" style="color:#cbd5e1;">Uygulama Adı:</label>
                            <div class="col-sm-9"><input type="text" name="custom_name" id="custom_name" class="form-control" placeholder="Örn: WeTransfer, Zoom" style="background:#0f172a; color:#fff; border:1px solid #475569;"></div>
                        </div>
                        <div class="form-group" style="margin-bottom:12px;">
                            <label class="col-sm-3 control-label" style="color:#cbd5e1;">Kategori:</label>
                            <div class="col-sm-9"><input type="text" name="custom_cat" id="custom_cat" class="form-control" placeholder="Örn: Dosya Transferi" style="background:#0f172a; color:#fff; border:1px solid #475569;"></div>
                        </div>
                        <div class="form-group" style="margin-bottom:12px;">
                            <label class="col-sm-3 control-label" style="color:#cbd5e1;">Alan Adları (Virgülle Ayrılmış):</label>
                            <div class="col-sm-9"><input type="text" name="custom_doms" id="custom_doms" class="form-control" placeholder="wetransfer.com, wetransfer.net" style="background:#0f172a; color:#fff; border:1px solid #475569;"></div>
                        </div>
                        <div class="form-group" style="margin-bottom:12px;">
                            <label class="col-sm-3 control-label" style="color:#cbd5e1;">Eylem:</label>
                            <div class="col-sm-9">
                                <select name="custom_action" class="form-control" style="background:#0f172a; color:#fff; border:1px solid #475569;">
                                    <option value="block">Engelle (Block)</option>
                                    <option value="allow">İzin Ver (Allow)</option>
                                </select>
                            </div>
                        </div>
                        <div class="text-right">
                            <button type="button" class="btn btn-default btn-sm" data-toggle="collapse" data-target="#customAppPanel" style="margin-right:6px;">Vazgeç</button>
                            <button type="submit" class="btn btn-success btn-sm"><i class="fa-solid fa-check"></i> İmzayı Ekle &amp; Uygula</button>
                        </div>
                    </form>
                </div>

                <?php if (empty($conf['custom_apps'])): ?>
                    <p class="text-muted text-center" style="margin:10px 0;">Henüz özel bir uygulama imzası eklenmedi.</p>
                <?php else: ?>
                    <table class="table table-striped table-condensed">
                        <thead>
                            <tr style="background:#090d16; color:#f8fafc;">
                                <th>Uygulama</th>
                                <th>Kategori</th>
                                <th>Alan Adları</th>
                                <th>Eylem</th>
                                <th style="text-align:right;">İşlem</th>
                            </tr>
                        </thead>
                        <tbody>
                            <?php foreach ($conf['custom_apps'] as $ca): ?>
                            <tr>
                                <td><strong><?=htmlspecialchars($ca['name'])?></strong></td>
                                <td><span class="label label-default"><?=htmlspecialchars($ca['category'])?></span></td>
                                <td><small class="font-monospace text-muted"><?=htmlspecialchars($ca['domains'])?></small></td>
                                <td>
                                    <?php if ($ca['action'] === 'block'): ?>
                                        <span class="label label-danger">Engelle</span>
                                    <?php else: ?>
                                        <span class="label label-success">İzin Ver</span>
                                    <?php endif; ?>
                                </td>
                                <td style="text-align:right;">
                                    <button type="button" onclick="deleteCustomApp('<?=htmlspecialchars($ca['id'])?>')" class="btn btn-xs btn-danger"><i class="fa-solid fa-trash"></i> Sil</button>
                                </td>
                            </tr>
                            <?php endforeach; ?>
                        </tbody>
                    </table>
                <?php endif; ?>
            </div>
        </div>

    </div>
</div>

<!-- Dedicated Action Form for Deletions -->
<form method="post" id="actionForm" style="display:none;" novalidate>
    <input type="hidden" name="act" id="action_act" value="">
    <input type="hidden" name="rule_id" id="action_rule_id" value="">
    <input type="hidden" name="custom_id" id="action_custom_id" value="">
</form>

<script>
// Styling helper
function styleSelector(el) {
    if (el.value === 'block') {
        el.style.backgroundColor = '#fee2e2';
        el.style.color = '#b91c1c';
    } else {
        el.style.backgroundColor = '#dcfce7';
        el.style.color = '#15803d';
    }
}

function setAllApps(action) {
    var selectors = document.querySelectorAll('.app-selector');
    selectors.forEach(function(sel) {
        sel.value = action;
        styleSelector(sel);
    });
}

function setCategoryApps(catKey, action) {
    var selectors = document.querySelectorAll('.app-selector[data-category="' + catKey + '"]');
    selectors.forEach(function(sel) {
        sel.value = action;
        styleSelector(sel);
    });
}

function blockAllVpns() {
    var selectors = document.querySelectorAll('.app-selector');
    selectors.forEach(function(sel) {
        var cat = sel.getAttribute('data-category');
        if (cat === 'vpn_tunnels' || cat === 'privacy_vpns' || cat === 'remote_access' || cat === 'gambling' || cat === 'dating') {
            sel.value = 'block';
            styleSelector(sel);
        }
    });
}

function setRecommended() {
    var recBlockCats = ['vpn_tunnels', 'privacy_vpns', 'p2p_sharing', 'gambling', 'dating', 'adult'];
    var recBlockApps = [
        'nordvpn', 'expressvpn', 'protonvpn', 'surfshark', 'openvpn', 'wireguard', 'tor', 'warp', 'psiphon', 'tailscale',
        'claudeflare', 'proton', 'icloud_private_relay', 'privacy',
        'bittorrent', 'tiktok', 'roblox', 'steam', 'epic_games', 'riot_games', 'league_of_legends', 'valorant', 'minecraft',
        'binance', 'betano', 'betfair', 'betway', 'blaze', 'tinder', 'plenty_of_fish', 'wizz', '4chan', 'onlyfans'
    ];
    var selectors = document.querySelectorAll('.app-selector');
    selectors.forEach(function(sel) {
        var name = sel.name.replace('apps[', '').replace(']', '');
        var cat = sel.getAttribute('data-category');
        if (recBlockApps.indexOf(name) !== -1 || recBlockCats.indexOf(cat) !== -1) {
            sel.value = 'block';
        } else {
            sel.value = 'allow';
        }
        styleSelector(sel);
    });
}

// Client Rule Target Type & Field Dynamic Switcher
var isManualMode = false;
function syncTargetVal(val) {
    document.getElementById('rule_target_val').value = (val || '').trim();
}

function onTargetTypeChange() {
    var type = document.getElementById('rule_target_type').value;
    isManualMode = false;
    document.getElementById('manualToggleLabel').innerText = 'Elle Yaz';

    var wraps = document.querySelectorAll('.target-field-wrap');
    for (var i = 0; i < wraps.length; i++) {
        wraps[i].style.display = 'none';
    }

    var targetWrap = document.getElementById('wrap_' + type);
    if (targetWrap) {
        targetWrap.style.display = 'block';
        var inputElem = targetWrap.querySelector('input, select');
        if (inputElem) {
            if (inputElem.value) {
                syncTargetVal(inputElem.value);
            }
            inputElem.focus();
        }
    }
}

function toggleManualTargetInput() {
    isManualMode = !isManualMode;
    var wraps = document.querySelectorAll('.target-field-wrap');
    for (var i = 0; i < wraps.length; i++) {
        wraps[i].style.display = 'none';
    }

    if (isManualMode) {
        document.getElementById('manualToggleLabel').innerText = 'Listeden Seç';
        document.getElementById('wrap_manual').style.display = 'block';
        var m = document.getElementById('sel_manual');
        m.value = document.getElementById('rule_target_val').value;
        m.focus();
    } else {
        document.getElementById('manualToggleLabel').innerText = 'Elle Yaz';
        onTargetTypeChange();
    }
}

function getActiveTargetVal() {
    if (isManualMode) {
        var m = document.getElementById('sel_manual');
        if (m && m.value.trim()) return m.value.trim();
    }
    var type = document.getElementById('rule_target_type').value;
    var wrap = document.getElementById('wrap_' + type);
    if (wrap) {
        var el = wrap.querySelector('input, select');
        if (el && el.value.trim()) return el.value.trim();
    }
    var h = document.getElementById('rule_target_val');
    return (h && h.value) ? h.value.trim() : '';
}

function validateRuleForm() {
    var val = getActiveTargetVal();
    document.getElementById('rule_target_val').value = val;
    if (!val) {
        alert('Lütfen kural için bir hedef değer (AD Grubu, Kullanıcı, Alias veya IP) seçiniz veya giriniz.');
        return false;
    }
    return true;
}

function validateCustomAppForm() {
    var name = document.getElementById('custom_name').value.trim();
    var doms = document.getElementById('custom_doms').value.trim();
    if (!name) {
        alert('Lütfen bir uygulama adı giriniz.');
        document.getElementById('custom_name').focus();
        return false;
    }
    if (!doms) {
        alert('Lütfen en az bir alan adı giriniz.');
        document.getElementById('custom_doms').focus();
        return false;
    }
    return true;
}

function openNewRuleForm() {
    document.getElementById('ruleFormTitle').innerText = 'Yeni Uygulama & VPN Kuralı Tanımla';
    document.getElementById('rule_id_input').value = '';
    document.getElementById('rule_target_type').value = 'group';
    document.getElementById('rule_target_val').value = '';
    document.getElementById('rule_app_target').selectedIndex = 0;
    document.getElementById('rule_action').value = 'block';
    document.getElementById('rule_descr').value = '';
    document.getElementById('ruleSubmitBtn').innerHTML = '<i class="fa-solid fa-check"></i> Kuralı Kaydet &amp; Uygula';
    onTargetTypeChange();
    $('#newRulePanel').collapse('show');
}

function editClientRule(r) {
    document.getElementById('ruleFormTitle').innerText = 'Kuralı Düzenle: ' + r.target_val;
    document.getElementById('rule_id_input').value = r.id;
    document.getElementById('rule_target_type').value = r.target_type;
    document.getElementById('rule_target_val').value = r.target_val;
    document.getElementById('rule_app_target').value = r.app_target;
    document.getElementById('rule_action').value = r.action || 'block';
    document.getElementById('rule_descr').value = r.descr || '';
    document.getElementById('ruleSubmitBtn').innerHTML = '<i class="fa-solid fa-check"></i> Değişiklikleri Güncelle';
    
    // Switch active wrap directly without clearing value
    isManualMode = false;
    document.getElementById('manualToggleLabel').innerText = 'Elle Yaz';
    var wraps = document.querySelectorAll('.target-field-wrap');
    for (var i = 0; i < wraps.length; i++) {
        wraps[i].style.display = 'none';
    }

    var wrap = document.getElementById('wrap_' + r.target_type);
    if (wrap) {
        wrap.style.display = 'block';
        var input = wrap.querySelector('input, select');
        if (input) {
            input.value = r.target_val;
            if (input.tagName === 'SELECT' && input.value !== r.target_val) {
                toggleManualTargetInput();
                document.getElementById('sel_manual').value = r.target_val;
            }
        }
    }
    document.getElementById('rule_target_val').value = r.target_val;

    $('#newRulePanel').collapse('show');
}

function deleteClientRule(id, target) {
    if (!confirm("'" + target + "' için tanımlı uygulama kuralını kaldırmak istediğinize emin misiniz?")) return;
    var f = document.getElementById('actionForm');
    document.getElementById('action_act').value = 'del_client_rule';
    document.getElementById('action_rule_id').value = id;
    f.submit();
}

function deleteCustomApp(id) {
    if (!confirm("Özel uygulama imzasını silmek istediğinize emin misiniz?")) return;
    var f = document.getElementById('actionForm');
    document.getElementById('action_act').value = 'del_custom_app';
    document.getElementById('action_custom_id').value = id;
    f.submit();
}
</script>

<?php include("foot.inc"); ?>
