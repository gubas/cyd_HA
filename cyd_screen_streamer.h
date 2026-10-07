#pragma once

#include "esphome.h"
#include <esp_http_server.h>

// =============================================================================
// CYD Screen Streamer & Web Simulator
// =============================================================================
// Exposes:
//   - GET / : ESPHome dashboard with prominent banner & link to Screen Simulator
//   - GET /screenshot (or /screenshot.bmp): Live 8-bit BMP framebuffer streaming (0 bytes RAM allocated)
//   - GET /screen (or /preview): Rich HTML5 CYD hardware simulator with live refresh & page controls
//   - GET /api/screen/page: Remote page navigation & menu toggle API
// =============================================================================

// Cast esp_display to access protected buffer_
using CydMipiDisplay = std::remove_pointer_t<decltype(esp_display)>;

class CydBufferAccessor : public CydMipiDisplay {
 public:
  static uint8_t *get_buffer(CydMipiDisplay *disp) {
    return static_cast<CydBufferAccessor *>(disp)->buffer_;
  }
};

// Access protected handlers_ to insert our handler at the front
struct AsyncWebServerAccessor : public web_server_idf::AsyncWebServer {
  static void insert_handler_front(web_server_idf::AsyncWebServer *srv, web_server_idf::AsyncWebHandler *h) {
    auto *accessor = static_cast<AsyncWebServerAccessor *>(srv);
    accessor->handlers_.insert(accessor->handlers_.begin(), h);
  }
};

static const char CYD_HOME_PAGE_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <link rel="icon" href="data:">
  <title>CYD HA Panel</title>
  <style>
    #cyd-nav-banner {
      position: sticky;
      top: 0;
      z-index: 99999;
      background: linear-gradient(135deg, #0b1120 0%, #162032 100%);
      color: #f8fafc;
      padding: 12px 20px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      border-bottom: 2px solid #38bdf8;
      box-shadow: 0 4px 20px rgba(0,0,0,0.5);
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
    }
    #cyd-nav-banner .brand {
      display: flex;
      align-items: center;
      gap: 10px;
      font-weight: 700;
      font-size: 1.05rem;
      letter-spacing: -0.3px;
    }
    #cyd-nav-banner .badge {
      font-size: 0.72rem;
      font-weight: 600;
      background: rgba(56, 189, 248, 0.15);
      color: #38bdf8;
      padding: 2px 8px;
      border-radius: 10px;
      border: 1px solid rgba(56, 189, 248, 0.3);
    }
    #cyd-nav-banner .btn-simulator {
      background: linear-gradient(135deg, #0284c7 0%, #38bdf8 100%);
      color: #0b1120;
      font-weight: 700;
      font-size: 0.88rem;
      padding: 8px 16px;
      border-radius: 10px;
      text-decoration: none;
      display: inline-flex;
      align-items: center;
      gap: 8px;
      box-shadow: 0 0 16px rgba(56, 189, 248, 0.4);
      transition: all 0.2s;
    }
    #cyd-nav-banner .btn-simulator:hover {
      transform: translateY(-2px);
      box-shadow: 0 0 24px rgba(56, 189, 248, 0.7);
    }
  </style>
</head>
<body>
  <nav id="cyd-nav-banner">
    <div class="brand">
      <span style="font-size: 1.3rem;">📺</span>
      <span>CYD HA Panel</span>
      <span class="badge">v4.2</span>
    </div>
    <a href="/screen" class="btn-simulator">
      <span>📱</span>
      <span>Simulateur d'écran &rarr;</span>
    </a>
  </nav>
  <esp-app></esp-app>
  <script src="https://oi.esphome.io/v3/www.js"></script>
</body>
</html>
)rawliteral";

static const char CYD_SIMULATOR_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>CYD HA Panel - Simulateur Écran</title>
<style>
  :root {
    --bg-dark: #0a0d14;
    --card-bg: rgba(22, 27, 39, 0.75);
    --border: rgba(255, 255, 255, 0.1);
    --accent: #38bdf8;
    --accent-glow: rgba(56, 189, 248, 0.35);
    --text: #f1f5f9;
    --text-muted: #94a3b8;
    --success: #22c55e;
  }
  * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; }
  body {
    background: radial-gradient(circle at 50% 20%, #172033 0%, var(--bg-dark) 100%);
    color: var(--text);
    min-height: 100vh;
    display: flex;
    flex-direction: column;
    align-items: center;
    padding: 20px 16px;
  }
  .top-nav-bar {
    width: 100%;
    max-width: 480px;
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 16px;
  }
  .nav-back-link {
    color: var(--text-muted);
    text-decoration: none;
    font-size: 0.85rem;
    font-weight: 600;
    display: inline-flex;
    align-items: center;
    gap: 6px;
    background: rgba(255, 255, 255, 0.05);
    padding: 7px 14px;
    border-radius: 10px;
    border: 1px solid var(--border);
    transition: all 0.2s;
  }
  .nav-back-link:hover {
    color: var(--text);
    background: rgba(255, 255, 255, 0.12);
    border-color: rgba(255, 255, 255, 0.25);
    transform: translateX(-2px);
  }
  .nav-badge {
    font-size: 0.75rem;
    font-weight: 600;
    color: var(--accent);
    background: rgba(56, 189, 248, 0.12);
    padding: 4px 10px;
    border-radius: 12px;
    border: 1px solid rgba(56, 189, 248, 0.25);
  }
  header {
    text-align: center;
    margin-bottom: 20px;
  }
  header h1 {
    font-size: 1.6rem;
    font-weight: 700;
    letter-spacing: -0.5px;
    background: linear-gradient(135deg, #fff 0%, var(--accent) 100%);
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
  }
  header p {
    color: var(--text-muted);
    font-size: 0.9rem;
    margin-top: 4px;
  }
  .main-container {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 20px;
    width: 100%;
    max-width: 480px;
  }
  /* CYD Hardware Enclosure */
  .cyd-enclosure {
    background: linear-gradient(145deg, #1e2433, #0f131c);
    border: 2px solid rgba(255, 255, 255, 0.12);
    box-shadow: 0 20px 40px rgba(0, 0, 0, 0.6), 0 0 0 1px rgba(0, 0, 0, 0.8), inset 0 1px 0 rgba(255, 255, 255, 0.15);
    border-radius: 36px;
    padding: 24px 20px 22px;
    position: relative;
    display: flex;
    flex-direction: column;
    align-items: center;
    user-select: none;
  }
  .cyd-top-bar {
    width: 100%;
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 0 8px 12px;
    font-size: 0.72rem;
    font-weight: 600;
    color: #64748b;
    letter-spacing: 0.5px;
    text-transform: uppercase;
  }
  .cyd-led {
    width: 8px;
    height: 8px;
    border-radius: 50%;
    background: var(--success);
    box-shadow: 0 0 8px var(--success);
    transition: all 0.3s;
  }
  .cyd-led.pulsing {
    animation: pulse 1s infinite alternate;
  }
  @keyframes pulse { from { opacity: 0.4; } to { opacity: 1; } }
  .screen-frame {
    position: relative;
    background: #000;
    border-radius: 12px;
    overflow: hidden;
    box-shadow: inset 0 0 10px rgba(0,0,0,0.9), 0 0 0 2px #0a0d14;
    cursor: pointer;
    line-height: 0;
  }
  #cyd-screen {
    width: 240px;
    height: 320px;
    display: block;
    image-rendering: pixelated;
    image-rendering: crisp-edges;
    transform: scale(1.15);
    transform-origin: top left;
  }
  .screen-wrapper {
    width: 276px;
    height: 368px;
    overflow: hidden;
    position: relative;
  }
  .screen-overlay {
    position: absolute;
    inset: 0;
    pointer-events: none;
    box-shadow: inset 0 0 12px rgba(0,0,0,0.5);
    background: linear-gradient(135deg, rgba(255,255,255,0.04) 0%, transparent 60%);
  }
  .touch-ripple {
    position: absolute;
    width: 30px;
    height: 30px;
    border-radius: 50%;
    background: rgba(56, 189, 248, 0.6);
    transform: translate(-50%, -50%) scale(0);
    animation: ripple 0.4s ease-out;
    pointer-events: none;
  }
  @keyframes ripple {
    to { transform: translate(-50%, -50%) scale(2.5); opacity: 0; }
  }
  .cyd-bottom-label {
    margin-top: 14px;
    font-size: 0.7rem;
    font-weight: 700;
    color: #475569;
    letter-spacing: 1.5px;
  }
  /* Controls card */
  .card {
    background: var(--card-bg);
    border: 1px solid var(--border);
    backdrop-filter: blur(12px);
    border-radius: 20px;
    padding: 18px 20px;
    width: 100%;
    display: flex;
    flex-direction: column;
    gap: 14px;
  }
  .card-row {
    display: flex;
    justify-content: space-between;
    align-items: center;
    gap: 12px;
  }
  .status-badge {
    display: inline-flex;
    align-items: center;
    gap: 6px;
    font-size: 0.8rem;
    font-weight: 500;
    color: var(--success);
    background: rgba(34, 197, 94, 0.12);
    padding: 4px 10px;
    border-radius: 12px;
  }
  .status-badge.paused {
    color: #f59e0b;
    background: rgba(245, 158, 11, 0.12);
  }
  .btn-group {
    display: flex;
    gap: 6px;
    flex-wrap: wrap;
    width: 100%;
  }
  button, .btn-link {
    background: rgba(255, 255, 255, 0.06);
    color: var(--text);
    border: 1px solid var(--border);
    border-radius: 10px;
    padding: 8px 14px;
    font-size: 0.82rem;
    font-weight: 600;
    cursor: pointer;
    transition: all 0.2s;
    text-decoration: none;
    display: inline-flex;
    align-items: center;
    justify-content: center;
    gap: 6px;
  }
  button:hover, .btn-link:hover {
    background: rgba(255, 255, 255, 0.12);
    border-color: rgba(255, 255, 255, 0.25);
    transform: translateY(-1px);
  }
  button:active { transform: translateY(0); }
  button.active {
    background: var(--accent);
    color: #0b1120;
    border-color: var(--accent);
    box-shadow: 0 0 12px var(--accent-glow);
  }
  .nav-btn { flex: 1; min-width: 70px; }
  .action-btn { flex: 1; }
  .telemetry {
    display: flex;
    justify-content: space-between;
    font-size: 0.75rem;
    color: var(--text-muted);
    border-top: 1px solid var(--border);
    padding-top: 10px;
  }
</style>
</head>
<body>

<div class="top-nav-bar">
  <a href="/" class="nav-back-link">&larr; &#127968; Tableau de bord ESPHome</a>
  <span class="nav-badge">CYD Panel v4.2</span>
</div>

<header>
  <h1>CYD Live Display</h1>
  <p>Simulation & contrôle en direct de l'écran ESP32</p>
</header>

<div class="main-container">
  <!-- CYD Enclosure -->
  <div class="cyd-enclosure">
    <div class="cyd-top-bar">
      <span>CYD 2.8" TFT</span>
      <div class="cyd-led pulsing" id="led-indicator"></div>
      <span id="page-label">PAGE 0</span>
    </div>
    
    <div class="screen-frame" id="screen-frame">
      <div class="screen-wrapper">
        <img id="cyd-screen" src="/screenshot.bmp" alt="CYD Display Framebuffer" />
        <div class="screen-overlay"></div>
      </div>
    </div>

    <div class="cyd-bottom-label">ESP32-2432S028R &bull; 240 &times; 320</div>
  </div>

  <!-- Navigation & Page Controls -->
  <div class="card">
    <div class="card-row">
      <div class="status-badge" id="status-badge">
        <span>&bull;</span> <span id="status-text">Flux actif (1s)</span>
      </div>
      <div style="display: flex; gap: 6px;">
        <button id="toggle-pause-btn" onclick="togglePause()">Pause</button>
        <button onclick="refreshScreen(true)">&#x21bb;</button>
      </div>
    </div>

    <div class="btn-group">
      <button class="nav-btn" onclick="switchPage(0)">Accueil</button>
      <button class="nav-btn" onclick="switchPage(1)">Capteurs</button>
      <button class="nav-btn" onclick="switchPage(2)">Énergie</button>
      <button class="nav-btn" onclick="switchPage(3)">Média</button>
      <button class="nav-btn" onclick="switchPage(4)">3D Print</button>
    </div>

    <div class="btn-group">
      <button class="action-btn" onclick="sendAction('prev')">&larr; Page Précédente</button>
      <button class="action-btn" onclick="sendAction('toggle_menu')">&#9776; Menu / Retour</button>
      <button class="action-btn" onclick="sendAction('next')">Page Suivante &rarr;</button>
    </div>

    <div class="card-row" style="margin-top: 4px;">
      <a class="btn-link" style="flex:1;" href="/screenshot.bmp" target="_blank" download="cyd_screenshot.bmp">&#128247; Télécharger BMP</a>
      <a class="btn-link" style="flex:1;" href="/">&#127968; Tableau de bord ESPHome</a>
    </div>

    <div class="telemetry">
      <span id="fps-stat">Délai: ~60ms</span>
      <span id="update-stat">Mis à jour: --</span>
    </div>
  </div>
</div>

<script>
  let isPaused = false;
  let refreshInterval = 1000;
  let timer = null;
  const screenImg = document.getElementById('cyd-screen');
  const led = document.getElementById('led-indicator');
  const statusBadge = document.getElementById('status-badge');
  const statusText = document.getElementById('status-text');
  const pauseBtn = document.getElementById('toggle-pause-btn');
  const pageLabel = document.getElementById('page-label');
  const updateStat = document.getElementById('update-stat');
  const fpsStat = document.getElementById('fps-stat');
  const screenFrame = document.getElementById('screen-frame');

  const pageNames = ["0 - ACCUEIL", "1 - CAPTEURS", "2 - ÉNERGIE", "3 - MÉDIA", "4 - 3D PRINT"];

  function refreshScreen(manual = false) {
    if (isPaused && !manual) return;
    const start = performance.now();
    const newImg = new Image();
    newImg.src = '/screenshot.bmp?t=' + Date.now();
    newImg.onload = () => {
      screenImg.src = newImg.src;
      const duration = Math.round(performance.now() - start);
      fpsStat.textContent = 'Transfert: ' + duration + ' ms';
      updateStat.textContent = new Date().toLocaleTimeString();
      led.classList.add('pulsing');
    };
    newImg.onerror = () => {
      fpsStat.textContent = 'Erreur réseau';
      led.classList.remove('pulsing');
    };
  }

  function togglePause() {
    isPaused = !isPaused;
    if (isPaused) {
      pauseBtn.textContent = 'Reprendre';
      statusBadge.classList.add('paused');
      statusText.textContent = 'En pause';
      clearInterval(timer);
    } else {
      pauseBtn.textContent = 'Pause';
      statusBadge.classList.remove('paused');
      statusText.textContent = 'Flux actif (1s)';
      refreshScreen(true);
      timer = setInterval(refreshScreen, refreshInterval);
    }
  }

  async function switchPage(page) {
    pageLabel.textContent = pageNames[page] || ('PAGE ' + page);
    await fetch('/api/screen/page?page=' + page);
    setTimeout(() => refreshScreen(true), 550);
  }

  async function sendAction(action) {
    const res = await fetch('/api/screen/page?action=' + action);
    try {
      const data = await res.json();
      pageLabel.textContent = data.menu ? 'MENU CONFIG' : (pageNames[data.page] || ('PAGE ' + data.page));
    } catch(e) {}
    setTimeout(() => refreshScreen(true), 550);
  }

  // Interactive Touch on Virtual Screen
  screenFrame.addEventListener('click', (e) => {
    const rect = screenFrame.getBoundingClientRect();
    const clickY = (e.clientY - rect.top) / rect.height;
    
    // Create ripple effect
    const ripple = document.createElement('div');
    ripple.className = 'touch-ripple';
    ripple.style.left = (e.clientX - rect.left) + 'px';
    ripple.style.top = (e.clientY - rect.top) + 'px';
    screenFrame.appendChild(ripple);
    setTimeout(() => ripple.remove(), 400);

    // If click near bottom dots (pagination area)
    if (clickY > 0.88) {
      sendAction('next');
    } else {
      sendAction('toggle_menu');
    }
  });

  // Start polling
  timer = setInterval(refreshScreen, refreshInterval);
  refreshScreen(true);
</script>
</body>
</html>
)rawliteral";

class CydScreenWebHandler : public web_server_idf::AsyncWebHandler {
 public:
  bool canHandle(web_server_idf::AsyncWebServerRequest *request) const override {
    char url_buf[web_server_idf::AsyncWebServerRequest::URL_BUF_SIZE];
    StringRef url = request->url_to(url_buf);
    return (url == "/" ||
            url == "/screenshot" || url == "/screenshot.bmp" || 
            url == "/screen" || url == "/preview" || 
            url == "/api/screen/page");
  }

  void handleRequest(web_server_idf::AsyncWebServerRequest *request) override {
    char url_buf[web_server_idf::AsyncWebServerRequest::URL_BUF_SIZE];
    StringRef url = request->url_to(url_buf);
    
    if (url == "/") {
      this->handle_home_page(request);
    } else if (url == "/screenshot" || url == "/screenshot.bmp") {
      this->handle_screenshot(request);
    } else if (url == "/screen" || url == "/preview") {
      this->handle_screen_simulator(request);
    } else if (url == "/api/screen/page") {
      this->handle_api_page(request);
    }
  }

 protected:
  void handle_home_page(web_server_idf::AsyncWebServerRequest *request) {
    httpd_req_t *r = *request;
    httpd_resp_set_status(r, HTTPD_200);
    httpd_resp_set_type(r, "text/html; charset=utf-8");
    httpd_resp_set_hdr(r, "Cache-Control", "no-cache");
    httpd_resp_send(r, CYD_HOME_PAGE_HTML, HTTPD_RESP_USE_STRLEN);
  }

  void handle_screenshot(web_server_idf::AsyncWebServerRequest *request) {
    if (request->hasArg("page")) {
      int p = atoi(request->arg("page").c_str());
      if (p >= 0 && p <= 4) {
        current_page->value() = p;
        show_return_page->value() = false;
      }
    }

    uint8_t *raw_buf = CydBufferAccessor::get_buffer(esp_display);
    if (raw_buf == nullptr) {
      request->send(500, "text/plain", "Display buffer not ready");
      return;
    }

    httpd_req_t *r = *request;
    httpd_resp_set_status(r, HTTPD_200);
    httpd_resp_set_type(r, "image/bmp");
    httpd_resp_set_hdr(r, "Cache-Control", "no-cache, no-store, must-revalidate");
    httpd_resp_set_hdr(r, "Pragma", "no-cache");
    httpd_resp_set_hdr(r, "Expires", "0");
    httpd_resp_set_hdr(r, "Access-Control-Allow-Origin", "*");

    // BMP Header (54 bytes) + Palette (1024 bytes) = 1078 bytes
    uint8_t header[1078];
    memset(header, 0, sizeof(header));

    // File Header (14 bytes)
    uint32_t file_size = 14 + 40 + 1024 + (240 * 320);
    uint32_t data_offset = 14 + 40 + 1024;
    header[0] = 'B';
    header[1] = 'M';
    memcpy(header + 2, &file_size, 4);
    memcpy(header + 10, &data_offset, 4);

    // BITMAPINFOHEADER (40 bytes)
    uint32_t biSize = 40;
    int32_t biWidth = 240;
    int32_t biHeight = 320; // positive = bottom-up
    uint16_t biPlanes = 1;
    uint16_t biBitCount = 8;
    uint32_t biCompression = 0;
    uint32_t biSizeImage = 240 * 320;
    int32_t biXPelsPerMeter = 2835;
    int32_t biYPelsPerMeter = 2835;
    uint32_t biClrUsed = 256;
    uint32_t biClrImportant = 0;

    memcpy(header + 14, &biSize, 4);
    memcpy(header + 18, &biWidth, 4);
    memcpy(header + 22, &biHeight, 4);
    memcpy(header + 26, &biPlanes, 2);
    memcpy(header + 28, &biBitCount, 2);
    memcpy(header + 30, &biCompression, 4);
    memcpy(header + 34, &biSizeImage, 4);
    memcpy(header + 38, &biXPelsPerMeter, 4);
    memcpy(header + 42, &biYPelsPerMeter, 4);
    memcpy(header + 46, &biClrUsed, 4);
    memcpy(header + 50, &biClrImportant, 4);

    // Palette (1024 bytes): RGB332 to B, G, R, 0
    uint8_t *pal = header + 54;
    for (int i = 0; i < 256; i++) {
      uint8_t r3 = (i >> 5) & 7;
      uint8_t g3 = (i >> 2) & 7;
      uint8_t b2 = i & 3;
      pal[i * 4 + 0] = (b2 * 255) / 3;
      pal[i * 4 + 1] = (g3 * 255) / 7;
      pal[i * 4 + 2] = (r3 * 255) / 7;
      pal[i * 4 + 3] = 0;
    }

    httpd_resp_send_chunk(r, reinterpret_cast<const char *>(header), sizeof(header));

    // Send lines bottom-up: from line 319 down to 0
    for (int y = 319; y >= 0; y--) {
      httpd_resp_send_chunk(r, reinterpret_cast<const char *>(raw_buf + y * 240), 240);
    }

    httpd_resp_send_chunk(r, nullptr, 0);
  }

  void handle_screen_simulator(web_server_idf::AsyncWebServerRequest *request) {
    httpd_req_t *r = *request;
    httpd_resp_set_status(r, HTTPD_200);
    httpd_resp_set_type(r, "text/html; charset=utf-8");
    httpd_resp_set_hdr(r, "Cache-Control", "no-cache");
    httpd_resp_send(r, CYD_SIMULATOR_HTML, HTTPD_RESP_USE_STRLEN);
  }

  void handle_api_page(web_server_idf::AsyncWebServerRequest *request) {
    if (request->hasArg("page")) {
      int p = atoi(request->arg("page").c_str());
      if (p >= 0 && p <= 4) {
        current_page->value() = p;
        show_return_page->value() = false;
      }
    }
    if (request->hasArg("menu")) {
      show_return_page->value() = (request->arg("menu") == "1" || request->arg("menu") == "true");
      menu_display_time->value() = millis();
    }
    if (request->hasArg("action")) {
      std::string act = request->arg("action");
      if (act == "next") {
        int p = (current_page->value() + 1) % 5;
        current_page->value() = p;
        show_return_page->value() = false;
      } else if (act == "prev") {
        int p = (current_page->value() + 4) % 5;
        current_page->value() = p;
        show_return_page->value() = false;
      } else if (act == "toggle_menu") {
        show_return_page->value() = !show_return_page->value();
        menu_display_time->value() = millis();
      }
    }

    std::string json = "{\"page\":";
    json += std::to_string(current_page->value());
    json += ",\"menu\":";
    json += show_return_page->value() ? "true" : "false";
    json += "}";

    request->send(200, "application/json", json.c_str());
  }
};

inline void init_cyd_screen_streamer() {
  if (web_server_base::global_web_server_base != nullptr) {
    web_server_base::global_web_server_base->init();
    auto *srv = web_server_base::global_web_server_base->get_server();
    if (srv != nullptr) {
      AsyncWebServerAccessor::insert_handler_front(srv, new CydScreenWebHandler());
      ESP_LOGI("cyd_screen", "CYD Screen Streamer installed at front of handlers (/, /screen, /screenshot)");
    }
  }
}
