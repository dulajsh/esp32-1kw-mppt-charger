#ifndef WEB_PAGE_H
#define WEB_PAGE_H

#include <Arduino.h>

const char WEB_PAGE_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ESP32 1kW MPPT / PSU Monitor</title>
<style>
* { box-sizing: border-box; margin: 0; padding: 0; }
body {
  font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
  background: #0b0f19;
  color: #e2e8f0;
  padding: 14px;
  line-height: 1.4;
}
.container { max-width: 980px; margin: 0 auto; }
.header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  flex-wrap: wrap;
  gap: 10px;
  margin-bottom: 16px;
  padding-bottom: 12px;
  border-bottom: 1px solid #1e293b;
}
.title-group h1 {
  font-size: 1.25rem;
  font-weight: 700;
  color: #f8fafc;
  letter-spacing: -0.02em;
}
.title-group .sub {
  font-size: 0.78rem;
  color: #94a3b8;
  margin-top: 2px;
}
.badge-group { display: flex; align-items: center; gap: 8px; flex-wrap: wrap; }
.badge {
  display: inline-flex;
  align-items: center;
  gap: 6px;
  padding: 4px 10px;
  border-radius: 9999px;
  font-size: 0.75rem;
  font-weight: 600;
  letter-spacing: 0.03em;
}
.badge-psu { background: rgba(59,130,246,0.18); color: #60a5fa; border: 1px solid rgba(59,130,246,0.35); }
.badge-charger { background: rgba(245,158,11,0.18); color: #fbbf24; border: 1px solid rgba(245,158,11,0.35); }
.badge-ok { background: rgba(16,185,129,0.18); color: #34d399; border: 1px solid rgba(16,185,129,0.35); }
.badge-err { background: rgba(239,68,68,0.22); color: #f87171; border: 1px solid rgba(239,68,68,0.4); }
.pulse { width: 8px; height: 8px; border-radius: 50%; background: currentColor; box-shadow: 0 0 8px currentColor; animation: pulse 2s infinite; }
@keyframes pulse { 0%, 100% { opacity: 1; } 50% { opacity: 0.35; } }

.grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(290px, 1fr));
  gap: 12px;
  margin-bottom: 12px;
}
.card {
  background: #131b2e;
  border: 1px solid #1e293b;
  border-radius: 12px;
  padding: 16px;
  box-shadow: 0 4px 12px rgba(0,0,0,0.25);
}
.card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  font-size: 0.75rem;
  font-weight: 700;
  text-transform: uppercase;
  letter-spacing: 0.06em;
  color: #94a3b8;
  margin-bottom: 10px;
}
.metric-hero {
  display: flex;
  align-items: baseline;
  gap: 6px;
  margin: 6px 0 12px 0;
}
.hero-val {
  font-size: 2.2rem;
  font-weight: 700;
  font-family: ui-monospace, SFMono-Regular, Menlo, Consolas, monospace;
  color: #f8fafc;
}
.hero-unit {
  font-size: 1rem;
  font-weight: 600;
  color: #94a3b8;
}
.table-data {
  width: 100%;
  border-collapse: collapse;
  font-size: 0.84rem;
}
.table-data td {
  padding: 5px 0;
  border-bottom: 1px solid rgba(30,41,59,0.6);
}
.table-data tr:last-child td { border-bottom: none; }
.table-data td.lbl { color: #94a3b8; }
.table-data td.val {
  text-align: right;
  font-family: ui-monospace, SFMono-Regular, Menlo, Consolas, monospace;
  font-weight: 600;
  color: #f1f5f9;
}
.progress-bar-bg {
  width: 100%;
  height: 8px;
  background: #1e293b;
  border-radius: 9999px;
  overflow: hidden;
  margin: 6px 0 10px 0;
}
.progress-bar-fill {
  height: 100%;
  width: 0%;
  background: linear-gradient(90deg, #3b82f6, #06b6d4);
  transition: width 0.3s ease;
}
.tag-group { display: flex; flex-wrap: wrap; gap: 6px; margin-top: 10px; }
.tag {
  font-size: 0.72rem;
  font-family: ui-monospace, monospace;
  font-weight: 600;
  padding: 3px 8px;
  border-radius: 6px;
  background: #1e293b;
  color: #64748b;
  border: 1px solid #334155;
  transition: all 0.2s ease;
}
.tag.warn {
  background: rgba(239,68,68,0.22);
  color: #f87171;
  border-color: rgba(239,68,68,0.45);
}
.tag.good {
  color: #94a3b8;
}
.footer {
  display: flex;
  justify-content: space-between;
  align-items: center;
  flex-wrap: wrap;
  gap: 8px;
  font-size: 0.72rem;
  color: #64748b;
  margin-top: 16px;
  padding-top: 12px;
  border-top: 1px solid #1e293b;
}
</style>
</head>
<body>
<div class="container">
  <div class="header">
    <div class="title-group">
      <h1>1kW MPPT / PSU Controller</h1>
      <div class="sub" id="hostIp">fugu-mppt-1kw.local</div>
    </div>
    <div class="badge-group">
      <span class="badge badge-psu" id="modeBadge"><span class="pulse"></span><span id="modeText">CONNECTING...</span></span>
      <span class="badge badge-ok" id="sysHealth">NORMAL</span>
    </div>
  </div>

  <div class="grid">
    <!-- Card 1: Input / Solar -->
    <div class="card">
      <div class="card-header">
        <span>Solar / Input Power</span>
        <span id="srcTag">SOURCE OK</span>
      </div>
      <div class="metric-hero">
        <span class="hero-val" id="pin" style="color:#fbbf24;">0.0</span>
        <span class="hero-unit">W</span>
      </div>
      <table class="table-data">
        <tr><td class="lbl">Input Voltage (Vin)</td><td class="val"><span id="vin">0.00</span> V</td></tr>
        <tr><td class="lbl">Input Current (Iin)</td><td class="val"><span id="iin">0.00</span> A</td></tr>
      </table>
    </div>

    <!-- Card 2: Output / Load -->
    <div class="card">
      <div class="card-header">
        <span>Output / Load</span>
        <span id="statusTag">REGULATING</span>
      </div>
      <div class="metric-hero">
        <span class="hero-val" id="pout" style="color:#38bdf8;">0.0</span>
        <span class="hero-unit">W</span>
      </div>
      <table class="table-data">
        <tr><td class="lbl">Output Voltage (Vout)</td><td class="val"><span id="vout">0.00</span> V</td></tr>
        <tr><td class="lbl">Output Current (Iout)</td><td class="val"><span id="iout">0.00</span> A</td></tr>
        <tr><td class="lbl">Target Setpoint</td><td class="val"><span id="targetV">0.00</span> V / <span id="limitA">0.00</span> A</td></tr>
      </table>
    </div>

    <!-- Card 3: Converter & Thermal -->
    <div class="card">
      <div class="card-header">
        <span>Efficiency & Converter</span>
        <span id="loopHz">0 Hz</span>
      </div>
      <div class="metric-hero">
        <span class="hero-val" id="eff" style="color:#34d399;">0.0</span>
        <span class="hero-unit">%</span>
      </div>
      <table class="table-data">
        <tr><td class="lbl">PWM Duty Cycle</td><td class="val"><span id="pwm">0</span> / <span id="pwmMax">0</span> (<span id="pwmPct">0.0</span>%)</td></tr>
        <tr><td class="lbl">Heatsink Temperature</td><td class="val"><span id="temp">0</span> &deg;C</td></tr>
        <tr><td class="lbl">Cooling Fan</td><td class="val" id="fan">OFF</td></tr>
        <tr><td class="lbl">Core 1 Power Loop</td><td class="val"><span id="loop">0.00</span> ms</td></tr>
      </table>
    </div>

    <!-- Card 4: Battery & Energy -->
    <div class="card">
      <div class="card-header">
        <span>Energy & Battery</span>
        <span id="presetTag">CUSTOM</span>
      </div>
      <div class="metric-hero">
        <span class="hero-val" id="wh" style="color:#a78bfa;">0.0</span>
        <span class="hero-unit">Wh</span>
      </div>
      <div class="progress-bar-bg"><div class="progress-bar-fill" id="socBar"></div></div>
      <table class="table-data">
        <tr><td class="lbl">Battery SOC / Percent</td><td class="val"><span id="soc">0</span> %</td></tr>
        <tr><td class="lbl">Total Energy (kWh)</td><td class="val"><span id="kwh">0.00</span> kWh</td></tr>
        <tr><td class="lbl">Cost Savings</td><td class="val">$ <span id="savings">0.00</span></td></tr>
        <tr><td class="lbl">System Uptime</td><td class="val" id="uptime">0d 0h 0m</td></tr>
      </table>
    </div>
  </div>

  <!-- Card 5: Protection & Health Diagnostics -->
  <div class="card">
    <div class="card-header">
      <span>Protection Diagnostics</span>
      <span id="adsStatus">ADS1115: OK</span>
    </div>
    <div class="tag-group">
      <span class="tag good" id="f_iuv">IUV: 0</span>
      <span class="tag good" id="f_oov">OOV: 0</span>
      <span class="tag good" id="f_ioc">IOC: 0</span>
      <span class="tag good" id="f_ooc">OOC: 0</span>
      <span class="tag good" id="f_ote">OTE: 0</span>
      <span class="tag good" id="f_bnc">BNC: 0</span>
      <span class="tag good" id="f_flv">FLV: 0</span>
    </div>
  </div>

  <div class="footer">
    <span>ESP32 1kW MPPT Firmware</span>
    <span>Auto-refresh: 1s live</span>
  </div>
</div>

<script>
function fmtTime(s) {
  var d = Math.floor(s / 86400);
  var h = Math.floor((s % 86400) / 3600);
  var m = Math.floor((s % 3600) / 60);
  return d + 'd ' + h + 'h ' + m + 'm';
}

function updateData() {
  fetch('/?data=1')
    .then(function(r) { return r.json(); })
    .then(function(d) {
      document.getElementById('vin').textContent = d.vin.toFixed(2);
      document.getElementById('iin').textContent = d.iin.toFixed(2);
      document.getElementById('pin').textContent = d.pin.toFixed(1);
      document.getElementById('vout').textContent = d.vout.toFixed(2);
      document.getElementById('iout').textContent = d.iout.toFixed(2);
      document.getElementById('pout').textContent = d.pout.toFixed(1);
      document.getElementById('eff').textContent = d.eff.toFixed(1);
      document.getElementById('temp').textContent = d.temp;
      document.getElementById('fan').textContent = d.fan ? 'ACTIVE' : 'OFF';
      document.getElementById('pwm').textContent = d.pwm;
      document.getElementById('pwmMax').textContent = d.pwmMax;
      var pwmPct = d.pwmMax > 0 ? ((d.pwm / d.pwmMax) * 100).toFixed(1) : '0.0';
      document.getElementById('pwmPct').textContent = pwmPct;
      document.getElementById('wh').textContent = d.wh.toFixed(1);
      document.getElementById('kwh').textContent = d.kwh.toFixed(2);
      document.getElementById('savings').textContent = d.savings.toFixed(2);
      document.getElementById('soc').textContent = d.soc;
      document.getElementById('socBar').style.width = Math.min(100, Math.max(0, d.soc)) + '%';
      document.getElementById('loop').textContent = d.loop.toFixed(2);
      var hz = d.loop > 0 ? Math.round(1000 / d.loop) : 0;
      document.getElementById('loopHz').textContent = hz + ' Hz';
      document.getElementById('uptime').textContent = fmtTime(d.uptime);
      document.getElementById('targetV').textContent = d.targetV.toFixed(2);
      document.getElementById('limitA').textContent = d.limitA.toFixed(2);
      document.getElementById('presetTag').textContent = d.preset;

      var modeText = document.getElementById('modeText');
      var modeBadge = document.getElementById('modeBadge');
      if (d.mode === 'PSU') {
        modeText.textContent = 'PSU MODE: ' + d.status;
        modeBadge.className = 'badge badge-psu';
      } else {
        modeText.textContent = 'CHARGER: ' + d.status;
        modeBadge.className = 'badge badge-charger';
      }
      document.getElementById('statusTag').textContent = d.status;

      var sysHealth = document.getElementById('sysHealth');
      if (d.err > 0) {
        sysHealth.textContent = 'FAULT (' + d.err + ')';
        sysHealth.className = 'badge badge-err';
      } else {
        sysHealth.textContent = 'NORMAL';
        sysHealth.className = 'badge badge-ok';
      }

      function updateFlag(id, label, val) {
        var el = document.getElementById(id);
        el.textContent = label + ': ' + val;
        el.className = 'tag ' + (val > 0 ? 'warn' : 'good');
      }
      updateFlag('f_iuv', 'IUV', d.iuv);
      updateFlag('f_oov', 'OOV', d.oov);
      updateFlag('f_ioc', 'IOC', d.ioc);
      updateFlag('f_ooc', 'OOC', d.ooc);
      updateFlag('f_ote', 'OTE', d.ote);
      updateFlag('f_bnc', 'BNC', d.bnc);
      updateFlag('f_flv', 'FLV', d.flv);

      var adsStatus = document.getElementById('adsStatus');
      adsStatus.textContent = d.ads ? 'ADS1115: OK' : 'ADS1115: NC';
      adsStatus.style.color = d.ads ? '#94a3b8' : '#f87171';
    })
    .catch(function(e) {
      console.error(e);
      document.getElementById('modeText').textContent = 'RECONNECTING...';
    });
}
setInterval(updateData, 1000);
updateData();
</script>
</body>
</html>
)rawliteral";

#endif
