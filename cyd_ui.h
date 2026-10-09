#pragma once

#include "esphome.h"
#include <string>
#include <map>

// =============================================================================
// CYD HA Panel - UI Engine & Shared Logic
// =============================================================================
// Features:
//   - Zero-allocation rain forecast cache updated on sensor event
//   - Unified page navigation & active state validation (hardware + web simulator)
//   - Modular rendering routines for all display screens
// =============================================================================

#define CYD_VERSION "v4.6.0"

// -----------------------------------------------------------------------------
// 1. Rain Forecast Cache (Optimized for 0 RAM allocation per frame)
// -----------------------------------------------------------------------------
struct CydRainData {
  uint8_t levels[9] = {0}; // 0 = sec, 1 = faible, 2 = moderee, 3 = forte
  char next_rain[16] = {0}; // ex: "15 min" ou ""
  bool valid = false;
};

inline CydRainData g_cyd_rain;

inline std::string cyd_extract_dict_val(const std::string &data, const char *key) {
  if (data.empty()) return "";
  std::string search_key = std::string("'") + key + "'";
  size_t pos = data.find(search_key);
  if (pos == std::string::npos) {
    search_key = std::string("\"") + key + "\"";
    pos = data.find(search_key);
  }
  if (pos == std::string::npos) return "";

  size_t colon = data.find(':', pos);
  if (colon == std::string::npos) return "";

  size_t val_start = colon + 1;
  while (val_start < data.size() && (data[val_start] == ' ' || data[val_start] == '\'' || data[val_start] == '"')) {
    val_start++;
  }

  size_t val_end = val_start;
  while (val_end < data.size() && data[val_end] != ',' && data[val_end] != '\'' && data[val_end] != '"' && data[val_end] != '}') {
    val_end++;
  }

  return data.substr(val_start, val_end - val_start);
}

inline void cyd_update_rain_forecast(const std::string &data) {
  if (data.empty()) {
    g_cyd_rain.valid = false;
    return;
  }
  const char* intervals[] = {"0 min", "5 min", "10 min", "15 min", "20 min", "25 min", "35 min", "45 min", "55 min"};
  g_cyd_rain.next_rain[0] = '\0';
  bool found_next = false;

  for (int i = 0; i < 9; i++) {
    std::string val = cyd_extract_dict_val(data, intervals[i]);
    std::string lower;
    for (char c : val) lower += (char)tolower(c);

    if (lower.find("forte") != std::string::npos || lower.find("fort") != std::string::npos) {
      g_cyd_rain.levels[i] = 3; // Forte
    } else if (lower.find("mod") != std::string::npos) {
      g_cyd_rain.levels[i] = 2; // Modérée
    } else if (lower.find("faible") != std::string::npos) {
      g_cyd_rain.levels[i] = 1; // Faible
    } else {
      g_cyd_rain.levels[i] = 0; // Sec / Autre
    }

    if (!found_next && g_cyd_rain.levels[i] > 0) {
      strncpy(g_cyd_rain.next_rain, intervals[i], sizeof(g_cyd_rain.next_rain) - 1);
      g_cyd_rain.next_rain[sizeof(g_cyd_rain.next_rain) - 1] = '\0';
      found_next = true;
    }
  }
  g_cyd_rain.valid = true;
}

// -----------------------------------------------------------------------------
// 2. Active States & Unified Navigation
// -----------------------------------------------------------------------------
inline bool cyd_is_media_active() {
  return media_state->has_state() && (media_state->state == "playing" || media_state->state == "paused");
}

inline bool cyd_is_printer_on() {
  return printer_switch_state->has_state() && printer_switch_state->state == "on";
}

inline void cyd_next_page() {
  int cur = current_page->value();
  bool media = cyd_is_media_active();
  bool printer = cyd_is_printer_on();
  int next = cur + 1;
  if (next == 3 && !media) next++;
  if (next == 4 && !printer) next++;
  if (next > 4) next = 0;
  current_page->value() = next;
  show_return_page->value() = false;
}

inline void cyd_prev_page() {
  int cur = current_page->value();
  bool media = cyd_is_media_active();
  bool printer = cyd_is_printer_on();
  int prev = cur - 1;
  if (prev < 0) prev = 4;
  if (prev == 4 && !printer) prev--;
  if (prev == 3 && !media) prev--;
  if (prev < 0) prev = 2;
  current_page->value() = prev;
  show_return_page->value() = false;
}

inline void cyd_validate_current_page() {
  int cur = current_page->value();
  bool media = cyd_is_media_active();
  bool printer = cyd_is_printer_on();
  if (cur == 3 && !media) current_page->value() = 0;
  if (cur == 4 && !printer) current_page->value() = 0;
  if (cur > 4) current_page->value() = 0;
}

// -----------------------------------------------------------------------------
// 3. Modular Screen Rendering Functions
// -----------------------------------------------------------------------------

inline void cyd_draw_wifi_setup(display::Display &it, const char* ap_ssid, const char* ap_password) {
  it.fill(black);
  it.printf(120, 15, info, blue, display::TextAlign::CENTER, "CONFIGURATION WI-FI");
  it.print(120, 35, buttons, white, display::TextAlign::CENTER, "Scannez pour configurer :");

  it.qr_code(75, 55, qr_wifi_setup, white, 3);

  const int BOX_X = 15;
  const int BOX_Y = 160;
  const int BOX_W = 210;
  const int BOX_H = 95;
  it.filled_rectangle(BOX_X, BOX_Y, BOX_W, BOX_H, card_bg);
  it.rectangle(BOX_X, BOX_Y, BOX_W, BOX_H, divider);

  it.print(BOX_X + 10, BOX_Y + 12, buttons, grey, "Point d'acces (AP) :");
  it.print(BOX_X + 10, BOX_Y + 28, info, white, ap_ssid);

  it.print(BOX_X + 10, BOX_Y + 50, buttons, grey, "Mot de passe :");
  it.print(BOX_X + 10, BOX_Y + 66, info, blue, ap_password);

  it.print(120, 275, buttons, white, display::TextAlign::CENTER, "Portail : http://192.168.4.1");
  it.print(120, 298, buttons, grey, display::TextAlign::CENTER, "Ou via USB sur web.esphome.io");
}

inline void cyd_draw_menu(display::Display &it, const char* const button_texts[8], const char* version_str) {
  it.fill(black);
  const int button_width = 100;
  const int button_height = 65;
  const int x_start = 15;
  const int y_start = 15;
  const int x_padding = 10;
  const int y_padding = 10;

  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 2; col++) {
      int idx = row * 2 + col;
      int x = x_start + col * (button_width + x_padding);
      int y = y_start + row * (button_height + y_padding);
      it.filled_rectangle(x, y, button_width, button_height, card_bg);
      it.rectangle(x, y, button_width, button_height, divider);
      int text_width = strlen(button_texts[idx]) * 5.5;
      int text_height = 16;
      it.print(x + (button_width - text_width) / 2,
               y + (button_height - text_height) / 2 + 20,
               buttons,
               white,
               button_texts[idx]);
    }
  }

  // Icons
  it.image(45, 20, up, (btn1_state->has_state() && btn1_state->state == "on") ? blue : grey);
  it.image(155, 20, meeting, (btn2_state->has_state() && btn2_state->state == "on") ? blue : grey);
  it.image(45, 95, stop, (btn3_state->has_state() && btn3_state->state == "on") ? blue : grey);
  it.image(155, 95, lampadaire, (btn4_state->has_state() && btn4_state->state == "on") ? blue : grey);
  it.image(45, 170, down, (btn5_state->has_state() && btn5_state->state == "on") ? blue : grey);
  it.image(155, 170, printer3d, (btn6_state->has_state() && btn6_state->state == "on") ? blue : grey);
  it.image(45, 245, bulb, (btn7_state->has_state() && btn7_state->state == "on") ? blue : grey);
  it.image(155, 245, back, grey);

  // Footer: IP address & firmware version
  if (cyd_ip_address->has_state() && cyd_ip_address->state != "") {
    it.printf(120, 313, buttons, grey, display::TextAlign::CENTER, "IP: %s  |  %s", cyd_ip_address->state.c_str(), version_str);
  } else {
    it.printf(120, 313, buttons, grey, display::TextAlign::CENTER, "CYD HA Panel | %s", version_str);
  }
}

inline void cyd_draw_header(display::Display &it, const char* device_friendly_name, bool show_boot_ip) {
  const int W = 240;
  const int HEADER_MARGIN = 10;
  const int HEADER_Y = 10;

  if (show_boot_ip) {
    it.printf(HEADER_MARGIN, HEADER_Y, info, blue, display::TextAlign::TOP_LEFT, "IP: %s", cyd_ip_address->state.c_str());
  } else {
    it.print(HEADER_MARGIN, HEADER_Y, info, white, display::TextAlign::TOP_LEFT, device_friendly_name);
  }
  it.strftime(W - HEADER_MARGIN, HEADER_Y, info, white, display::TextAlign::TOP_RIGHT, "%d/%m %H:%M", esptime->now());
}

inline void cyd_draw_weather_page(display::Display &it, const char* weather_title, const char* location_name,
                                  const char* no_alerts, const char* rain_prefix, const char* rain_none) {
  const int W = 240;
  const int CX = W / 2;
  const int TITLE_Y = 64;
  it.printf(CX, TITLE_Y - 10, info, white, display::TextAlign::CENTER, "%s — %s", weather_title, location_name);

  const int ICON_Y = 90;
  if (weather_state->has_state()) {
    static const std::map<std::string, std::string> weather_icon_map = {
      {"clear-night", "\U000F0594"}, {"cloudy", "\U000F0590"}, {"fog", "\U000F0591"},
      {"hail", "\U000F0592"}, {"lightning", "\U000F0593"}, {"lightning-rainy", "\U000F067E"},
      {"partlycloudy", "\U000F0595"}, {"pouring", "\U000F0596"}, {"rainy", "\U000F0597"},
      {"snowy", "\U000F0598"}, {"snowy-rainy", "\U000F067F"}, {"sunny", "\U000F0599"},
      {"windy", "\U000F059D"}, {"windy-variant", "\U000F059E"},
    };
    const std::string& ws = weather_state->state;
    auto itw = weather_icon_map.find(ws);
    const char* icon = (itw != weather_icon_map.end()) ? itw->second.c_str() : "\U000F0599";

    unsigned long t = millis();
    bool phase = ((t / 500) % 2) == 0;
    bool is_dynamic = (
      ws == "rainy" || ws == "pouring" ||
      ws == "lightning" || ws == "lightning-rainy" ||
      ws == "windy" || ws == "windy-variant" ||
      ws == "snowy" || ws == "snowy-rainy" ||
      ws == "hail"
    );

    if (phase) {
      if (ws == "rainy") icon = "\U000F0596";
      else if (ws == "pouring") icon = "\U000F0597";
      else if (ws == "lightning") icon = "\U000F067E";
      else if (ws == "lightning-rainy") icon = "\U000F0593";
      else if (ws == "windy") icon = "\U000F059E";
      else if (ws == "windy-variant") icon = "\U000F059D";
      else if (ws == "snowy") icon = "\U000F067F";
      else if (ws == "snowy-rainy") icon = "\U000F0598";
    }

    int icon_y = ICON_Y + (is_dynamic && phase ? 1 : 0);
    it.printf(50, icon_y, fontmeteo, blue, display::TextAlign::CENTER, icon);
  } else {
    it.printf(50, ICON_Y, fontmeteo, blue, display::TextAlign::CENTER, "\U000F0599");
  }

  // Temp & Humidity
  it.image(W - 100, ICON_Y - 10, temp_small, blue);
  if (temp_ext->has_state()) {
    it.printf(W - 70, ICON_Y, info, white, display::TextAlign::CENTER_LEFT, "%.1f°C", temp_ext->state);
  } else {
    it.print(W - 70, ICON_Y, info, white, display::TextAlign::CENTER_LEFT, "--°C");
  }

  it.image(W - 100, ICON_Y + 15, humidity_small, blue);
  if (humidity_ext->has_state()) {
    it.printf(W - 70, ICON_Y + 25, info, white, display::TextAlign::CENTER_LEFT, "%.0f %%", humidity_ext->state);
  }

  // Alerts
  const int ALERT_Y = 135;
  static char alert_buffer[100];
  alert_buffer[0] = '\0';
  bool has_alert = false;

  if (alert_vent->has_state() && alert_vent->state != "Vert" && alert_vent->state != "vert") {
    sprintf(alert_buffer + strlen(alert_buffer), "Vent(%s) ", alert_vent->state.c_str());
    has_alert = true;
  }
  if (alert_pluie->has_state() && alert_pluie->state != "Vert" && alert_pluie->state != "vert") {
    if (has_alert) strcat(alert_buffer, "| ");
    sprintf(alert_buffer + strlen(alert_buffer), "Pluie(%s) ", alert_pluie->state.c_str());
    has_alert = true;
  }
  if (alert_orages->has_state() && alert_orages->state != "Vert" && alert_orages->state != "vert") {
    if (has_alert) strcat(alert_buffer, "| ");
    sprintf(alert_buffer + strlen(alert_buffer), "Orage(%s) ", alert_orages->state.c_str());
    has_alert = true;
  }
  if (alert_neige->has_state() && alert_neige->state != "Vert" && alert_neige->state != "vert") {
    if (has_alert) strcat(alert_buffer, "| ");
    sprintf(alert_buffer + strlen(alert_buffer), "Neige(%s) ", alert_neige->state.c_str());
    has_alert = true;
  }
  if (alert_inondation->has_state() && alert_inondation->state != "Vert" && alert_inondation->state != "vert") {
    if (has_alert) strcat(alert_buffer, "| ");
    sprintf(alert_buffer + strlen(alert_buffer), "Inond(%s) ", alert_inondation->state.c_str());
    has_alert = true;
  }

  if (has_alert) {
    it.printf(CX, ALERT_Y, info, white, display::TextAlign::CENTER, "%s", alert_buffer);
  } else {
    it.print(CX, ALERT_Y, info, white, display::TextAlign::CENTER, no_alerts);
  }

  // Rain forecast bars (using precomputed g_cyd_rain - zero allocation!)
  const int RAIN_Y = ALERT_Y + 20;
  const int RECT_W_5 = 16;
  const int RECT_W_10 = 32;
  const int RECT_H = 8;
  const int RECT_SPACING = 3;
  const int TOTAL_RAIN_W = 6 * RECT_W_5 + 3 * RECT_W_10 + 8 * RECT_SPACING;
  const int RAIN_START_X = (W - TOTAL_RAIN_W) / 2;

  // Lazy update if not yet parsed
  if (!g_cyd_rain.valid && next_rain_forecast_data->has_state()) {
    cyd_update_rain_forecast(next_rain_forecast_data->state);
  }

  int rect_x = RAIN_START_X;
  for (int i = 0; i < 9; i++) {
    const int rect_w = (i < 6) ? RECT_W_5 : RECT_W_10;
    uint8_t lvl = g_cyd_rain.levels[i];
    if (lvl == 0) {
      it.rectangle(rect_x, RAIN_Y, rect_w, RECT_H, grey);
    } else {
      Color fill_col;
      if (lvl == 3) fill_col = Color(150, 50, 0);       // BGR Bleu fonce
      else if (lvl == 2) fill_col = Color(180, 100, 50); // BGR Bleu moyen
      else fill_col = Color(200, 150, 100);             // BGR Bleu clair
      it.filled_rectangle(rect_x, RAIN_Y, rect_w, RECT_H, fill_col);
      it.rectangle(rect_x, RAIN_Y, rect_w, RECT_H, grey);
    }
    rect_x += rect_w + RECT_SPACING;
  }

  // Time labels under bars
  const char* intervals_num[] = {"0", "5", "10", "15", "20", "25", "35", "45", "55"};
  int label_x = RAIN_START_X;
  for (int i = 0; i < 9; i++) {
    const int rect_w = (i < 6) ? RECT_W_5 : RECT_W_10;
    int cx = label_x + rect_w / 2;
    it.printf(cx, RAIN_Y + RECT_H + 1, info, grey, display::TextAlign::TOP_CENTER, "%s", intervals_num[i]);
    label_x += rect_w + RECT_SPACING;
  }

  // Next rain text
  const int NEXT_TEXT_Y = RAIN_Y + RECT_H + 30;
  if (g_cyd_rain.next_rain[0] != '\0') {
    it.printf(CX, NEXT_TEXT_Y, info, white, display::TextAlign::CENTER, "%s: %s", rain_prefix, g_cyd_rain.next_rain);
  } else {
    it.print(CX, NEXT_TEXT_Y, info, white, display::TextAlign::CENTER, rain_none);
  }

  // Grille 6 infos
  const int GRID_START_Y = 200;
  const int ROW_HEIGHT = 22;
  const int COL1_X = 20;
  const int COL2_X = 118;
  const int TEXT_OFFSET = 22;

  // Row 1: Rain | Wind
  it.image(COL1_X, GRID_START_Y, umbrella, blue);
  if (rainchance->has_state()) it.printf(COL1_X + TEXT_OFFSET, GRID_START_Y + 10, info, white, display::TextAlign::CENTER_LEFT, "%.0f %%", rainchance->state);
  else it.print(COL1_X + TEXT_OFFSET, GRID_START_Y + 10, info, white, display::TextAlign::CENTER_LEFT, "-- %");

  if (wind_bearing->has_state() && wind_speed->has_state()) {
    float bearing = wind_bearing->state;
    const char* direction;
    if (bearing >= 337.5 || bearing < 22.5) direction = "N";
    else if (bearing >= 22.5 && bearing < 67.5) direction = "NE";
    else if (bearing >= 67.5 && bearing < 112.5) direction = "E";
    else if (bearing >= 112.5 && bearing < 157.5) direction = "SE";
    else if (bearing >= 157.5 && bearing < 202.5) direction = "S";
    else if (bearing >= 202.5 && bearing < 247.5) direction = "SW";
    else if (bearing >= 247.5 && bearing < 292.5) direction = "W";
    else direction = "NW";
    it.printf(COL2_X, GRID_START_Y + 10, info, white, display::TextAlign::CENTER_LEFT, "%s %.0f km/h", direction, wind_speed->state);
  } else if (wind_speed->has_state()) {
    it.printf(COL2_X, GRID_START_Y + 10, info, white, display::TextAlign::CENTER_LEFT, "%.0f km/h", wind_speed->state);
  } else {
    it.print(COL2_X, GRID_START_Y + 10, info, white, display::TextAlign::CENTER_LEFT, "-- km/h");
  }

  // Row 2: Snow | Pressure
  it.image(COL1_X, GRID_START_Y + ROW_HEIGHT, snow_small, blue);
  if (snowchance->has_state()) it.printf(COL1_X + TEXT_OFFSET, GRID_START_Y + ROW_HEIGHT + 10, info, white, display::TextAlign::CENTER_LEFT, "%.0f %%", snowchance->state);
  else it.print(COL1_X + TEXT_OFFSET, GRID_START_Y + ROW_HEIGHT + 10, info, white, display::TextAlign::CENTER_LEFT, "-- %");

  it.image(COL2_X, GRID_START_Y + ROW_HEIGHT, pressure_small, blue);
  if (pressure->has_state()) it.printf(COL2_X + TEXT_OFFSET, GRID_START_Y + ROW_HEIGHT + 10, info, white, display::TextAlign::CENTER_LEFT, "%.0f hPa", pressure->state);
  else it.print(COL2_X + TEXT_OFFSET, GRID_START_Y + ROW_HEIGHT + 10, info, white, display::TextAlign::CENTER_LEFT, "-- hPa");

  // Row 3: Freeze
  it.image(COL1_X, GRID_START_Y + 2*ROW_HEIGHT, freeze_small, blue);
  if (freezechance->has_state()) it.printf(COL1_X + TEXT_OFFSET, GRID_START_Y + 2*ROW_HEIGHT + 10, info, white, display::TextAlign::CENTER_LEFT, "%.0f %%", freezechance->state);
  else it.print(COL1_X + TEXT_OFFSET, GRID_START_Y + 2*ROW_HEIGHT + 10, info, white, display::TextAlign::CENTER_LEFT, "-- %");

  // Row 4: Sunrise | Sunset
  it.image(COL1_X, GRID_START_Y + 3*ROW_HEIGHT, sunrise_small, blue);
  if (sunrise_time->has_state()) {
    std::string sunrise_str = sunrise_time->state;
    size_t time_pos = sunrise_str.find('T');
    if (time_pos != std::string::npos && sunrise_str.length() > time_pos + 5) {
      std::string time_part = sunrise_str.substr(time_pos + 1, 5);
      it.printf(COL1_X + TEXT_OFFSET, GRID_START_Y + 3*ROW_HEIGHT + 10, info, white, display::TextAlign::CENTER_LEFT, "%s", time_part.c_str());
    }
  }

  it.image(COL2_X, GRID_START_Y + 3*ROW_HEIGHT, sunset_small, blue);
  if (sunset_time->has_state()) {
    std::string sunset_str = sunset_time->state;
    size_t time_pos = sunset_str.find('T');
    if (time_pos != std::string::npos && sunset_str.length() > time_pos + 5) {
      std::string time_part = sunset_str.substr(time_pos + 1, 5);
      it.printf(COL2_X + TEXT_OFFSET, GRID_START_Y + 3*ROW_HEIGHT + 10, info, white, display::TextAlign::CENTER_LEFT, "%s", time_part.c_str());
    }
  }
}

inline void cyd_draw_sensors_page(display::Display &it,
                                  const char* l1, image::Image* ic1,
                                  const char* l2, image::Image* ic2,
                                  const char* l3, image::Image* ic3,
                                  const char* l4, image::Image* ic4) {
  const int W = 240;
  const int CARD_W = 105, CARD_H = 68;
  const int GAP = 8;
  const int GRID_X = (W - 2*CARD_W - GAP) / 2;
  const int GRID_Y = 50;

  // Bloc 1
  if (sensor_bloc1_temp->has_state()) {
    int x1 = GRID_X, y1 = GRID_Y;
    it.filled_rectangle(x1, y1, CARD_W, CARD_H, card_bg);
    it.rectangle(x1, y1, CARD_W, CARD_H, divider);
    it.image(x1 + 6, y1 + 8, ic1, blue);
    it.print(x1 + 6, y1 + 50, info, white, l1);
    it.printf(x1 + 48, y1 + 12, info, white, "%.1f C", sensor_bloc1_temp->state);
    if (sensor_bloc1_hum->has_state()) {
      it.printf(x1 + 48, y1 + 30, info, white, "%.0f %%", sensor_bloc1_hum->state);
    }
  }

  // Bloc 2
  if (sensor_bloc2_temp->has_state()) {
    int x2 = GRID_X + CARD_W + GAP, y2 = GRID_Y;
    it.filled_rectangle(x2, y2, CARD_W, CARD_H, card_bg);
    it.rectangle(x2, y2, CARD_W, CARD_H, divider);
    it.image(x2 + 6, y2 + 8, ic2, blue);
    it.print(x2 + 6, y2 + 50, info, white, l2);
    it.printf(x2 + 48, y2 + 12, info, white, "%.1f C", sensor_bloc2_temp->state);
    if (sensor_bloc2_hum->has_state()) {
      it.printf(x2 + 48, y2 + 30, info, white, "%.0f %%", sensor_bloc2_hum->state);
    }
  }

  // Bloc 3
  if (sensor_bloc3_temp->has_state()) {
    int x3 = GRID_X, y3 = GRID_Y + CARD_H + GAP;
    it.filled_rectangle(x3, y3, CARD_W, CARD_H, card_bg);
    it.rectangle(x3, y3, CARD_W, CARD_H, divider);
    it.image(x3 + 6, y3 + 8, ic3, blue);
    it.print(x3 + 6, y3 + 50, info, white, l3);
    it.printf(x3 + 48, y3 + 12, info, white, "%.1f C", sensor_bloc3_temp->state);
    if (sensor_bloc3_hum->has_state()) {
      it.printf(x3 + 48, y3 + 30, info, white, "%.0f %%", sensor_bloc3_hum->state);
    }
  }

  // Bloc 4
  if (sensor_bloc4_temp->has_state()) {
    int x4 = GRID_X + CARD_W + GAP, y4 = GRID_Y + CARD_H + GAP;
    it.filled_rectangle(x4, y4, CARD_W, CARD_H, card_bg);
    it.rectangle(x4, y4, CARD_W, CARD_H, divider);
    it.image(x4 + 6, y4 + 8, ic4, blue);
    it.print(x4 + 6, y4 + 50, info, white, l4);
    it.printf(x4 + 48, y4 + 12, info, white, "%.1f C", sensor_bloc4_temp->state);
    if (sensor_bloc4_hum->has_state()) {
      it.printf(x4 + 48, y4 + 30, info, white, "%.0f %%", sensor_bloc4_hum->state);
    }
  }
}

inline void cyd_draw_energy_page(display::Display &it,
                                 const char* l1, const char* l2,
                                 const char* l3, const char* l4) {
  const int W = 240;
  const int CARD_W = 105, CARD_H = 68;
  const int GAP = 8;
  const int GRID_X = (W - 2*CARD_W - GAP) / 2;
  const int GRID_Y = 50;

  // Bloc 1: Puissance globale maison
  int x1 = GRID_X, y1 = GRID_Y;
  it.filled_rectangle(x1, y1, CARD_W, CARD_H, card_bg);
  it.rectangle(x1, y1, CARD_W, CARD_H, divider);
  it.image(x1 + 6, y1 + 8, energy_power_icon, blue);
  it.print(x1 + 6, y1 + 50, info, white, l1);
  if (energy_power->has_state()) {
    float p = energy_power->state;
    if (p >= 1000.0f) it.printf(x1 + 48, y1 + 18, info, white, "%.2f kW", p / 1000.0f);
    else it.printf(x1 + 48, y1 + 18, info, white, "%.0f W", p);
  } else {
    it.print(x1 + 48, y1 + 18, info, white, "-- W");
  }

  // Bloc 2: Coût électricité du jour
  int x2 = GRID_X + CARD_W + GAP, y2 = GRID_Y;
  it.filled_rectangle(x2, y2, CARD_W, CARD_H, card_bg);
  it.rectangle(x2, y2, CARD_W, CARD_H, divider);
  it.image(x2 + 6, y2 + 8, energy_cost_icon, blue);
  it.print(x2 + 6, y2 + 50, info, white, l2);
  if (energy_cost->has_state()) {
    it.printf(x2 + 48, y2 + 18, info, white, "%.2f €", energy_cost->state);
  } else {
    it.print(x2 + 48, y2 + 18, info, white, "-- €");
  }

  // Bloc 3: Puissance clim
  int x3 = GRID_X, y3 = GRID_Y + CARD_H + GAP;
  it.filled_rectangle(x3, y3, CARD_W, CARD_H, card_bg);
  it.rectangle(x3, y3, CARD_W, CARD_H, divider);
  it.image(x3 + 6, y3 + 8, ac_power_icon, blue);
  it.print(x3 + 6, y3 + 50, info, white, l3);
  if (ac_power->has_state()) {
    float p = ac_power->state;
    if (p >= 1000.0f) it.printf(x3 + 48, y3 + 18, info, white, "%.2f kW", p / 1000.0f);
    else it.printf(x3 + 48, y3 + 18, info, white, "%.0f W", p);
  } else {
    it.print(x3 + 48, y3 + 18, info, white, "-- W");
  }

  // Bloc 4: Conso clim jour
  int x4 = GRID_X + CARD_W + GAP, y4 = GRID_Y + CARD_H + GAP;
  it.filled_rectangle(x4, y4, CARD_W, CARD_H, card_bg);
  it.rectangle(x4, y4, CARD_W, CARD_H, divider);
  it.image(x4 + 6, y4 + 8, ac_energy_icon, blue);
  it.print(x4 + 6, y4 + 50, info, white, l4);
  if (ac_energy->has_state()) {
    it.printf(x4 + 48, y4 + 18, info, white, "%.1f kWh", ac_energy->state);
  } else {
    it.print(x4 + 48, y4 + 18, info, white, "-- kWh");
  }
}

inline void cyd_draw_media_page(display::Display &it) {
  const int W = 240;
  const int CX = W / 2;
  const int ICON_Y = 50;
  it.image(CX - 25, ICON_Y, media_icon, blue);

  int current_y = 120;
  int max_width = 230;
  int line_height = date->get_height() + 2;

  auto print_wrap = [&](std::string text, Color color) {
    std::string remainder = text;
    while (remainder.length() > 0) {
      std::string line = remainder;
      int w, h, x_off, y_off;
      it.get_text_bounds(0, 0, line.c_str(), date, display::TextAlign::TOP_LEFT, &x_off, &y_off, &w, &h);
      while (line.length() > 0 && w > max_width) {
        size_t last_space = line.rfind(' ');
        if (last_space != std::string::npos) line = line.substr(0, last_space);
        else line = line.substr(0, line.length() - 1);
        it.get_text_bounds(0, 0, line.c_str(), date, display::TextAlign::TOP_LEFT, &x_off, &y_off, &w, &h);
      }
      it.print(CX, current_y, date, color, display::TextAlign::TOP_CENTER, line.c_str());
      current_y += line_height;
      if (line.length() < remainder.length()) {
        remainder = remainder.substr(line.length());
        if (remainder.length() > 0 && remainder[0] == ' ') remainder = remainder.substr(1);
      } else {
        remainder = "";
      }
    }
  };

  if (media_artist->has_state() && media_artist->state != "") {
    print_wrap(media_artist->state, grey);
  }
  current_y += 5;
  if (media_title->has_state() && media_title->state != "") {
    print_wrap(media_title->state, white);
  }
  current_y += 10;
  if (media_state->has_state()) {
    it.printf(CX, current_y, info, blue, display::TextAlign::TOP_CENTER, "[ %s ]", media_state->state.c_str());
  }
}

inline void cyd_draw_printer_page(display::Display &it, const char* title, const char* brand) {
  const int W = 240;
  const int CX = W / 2;
  const int TITLE_Y = 64;
  it.printf(CX, TITLE_Y - 10, info, white, display::TextAlign::CENTER, "%s — %s", title, brand);

  const int ICON_Y = 90;
  it.image(30, ICON_Y - 20, printer3d, blue);

  if (current_file->has_state()) {
    static char filename_buf[64];
    static int scroll_pos = 0;
    static unsigned long last_scroll = 0;
    const char* fname = current_file->state.c_str();
    int len = strlen(fname);
    const int MAX_DISPLAY_CHARS = 28;

    if (len <= MAX_DISPLAY_CHARS) {
      it.printf(W - 135, ICON_Y - 15, info, white, display::TextAlign::TOP_LEFT, "%s", fname);
      scroll_pos = 0;
    } else {
      if (millis() - last_scroll > 150) {
        scroll_pos++;
        if (scroll_pos > len - MAX_DISPLAY_CHARS) scroll_pos = 0;
        last_scroll = millis();
      }
      strncpy(filename_buf, fname + scroll_pos, MAX_DISPLAY_CHARS);
      filename_buf[MAX_DISPLAY_CHARS] = '\0';
      it.printf(W - 135, ICON_Y - 15, info, white, display::TextAlign::TOP_LEFT, "%s", filename_buf);
    }
  } else {
    it.print(W - 135, ICON_Y - 15, info, white, display::TextAlign::TOP_LEFT, "--");
  }

  if (print_status->has_state()) {
    it.printf(W - 135, ICON_Y + 5, info, white, display::TextAlign::TOP_LEFT, "%s", print_status->state.c_str());
  } else {
    it.print(W - 135, ICON_Y + 5, info, white, display::TextAlign::TOP_LEFT, "--");
  }

  // Progression bar
  const int BAR_Y = 130;
  const int BAR_W = 200, BAR_H = 24, BAR_X = (W - BAR_W) / 2;
  it.filled_rectangle(BAR_X, BAR_Y, BAR_W, BAR_H, card_bg);
  it.rectangle(BAR_X, BAR_Y, BAR_W, BAR_H, divider);
  if (print_progress->has_state()) {
    float p = print_progress->state;
    if (p < 0) p = 0; if (p > 100) p = 100;
    int fill = (int)((BAR_W - 4) * (p / 100.0f));
    if (fill > 0) it.filled_rectangle(BAR_X + 2, BAR_Y + 2, fill, BAR_H - 4, blue);
    it.printf(CX, BAR_Y + 12, info, white, display::TextAlign::CENTER, "%.0f%%", p);
  } else {
    it.print(CX, BAR_Y + 12, info, white, display::TextAlign::CENTER, "--%");
  }

  // Grille 2x2
  const int GRID_START_Y = 165;
  const int ROW_HEIGHT = 22;
  const int COL1_X = 15;
  const int COL2_X = 125;
  const int TEXT_OFFSET = 22;

  // Ligne 1: Temps restant | Heure de fin
  it.image(COL1_X, GRID_START_Y, timer_small, blue);
  if (time_remaining->has_state()) {
    it.printf(COL1_X + TEXT_OFFSET, GRID_START_Y + 10, info, white, display::TextAlign::CENTER_LEFT, "%s min", time_remaining->state.c_str());
  } else {
    it.print(COL1_X + TEXT_OFFSET, GRID_START_Y + 10, info, white, display::TextAlign::CENTER_LEFT, "-- min");
  }

  it.image(COL2_X, GRID_START_Y, clock_end_small, blue);
  if (end_time->has_state()) {
    std::string end_str = end_time->state;
    size_t space_pos = end_str.find(' ');
    if (space_pos != std::string::npos && end_str.length() > space_pos + 5) {
      std::string time_part = end_str.substr(space_pos + 1, 5);
      it.printf(COL2_X + TEXT_OFFSET, GRID_START_Y + 10, info, white, display::TextAlign::CENTER_LEFT, "%s", time_part.c_str());
    } else {
      it.printf(COL2_X + TEXT_OFFSET, GRID_START_Y + 10, info, white, display::TextAlign::CENTER_LEFT, "%s", end_time->state.c_str());
    }
  } else {
    it.print(COL2_X + TEXT_OFFSET, GRID_START_Y + 10, info, white, display::TextAlign::CENTER_LEFT, "--:--");
  }

  // Ligne 2: Temp buse | Temp lit
  it.image(COL1_X, GRID_START_Y + ROW_HEIGHT, nozzle_small, blue);
  if (nozzle_temp->has_state()) {
    if (nozzle_temp_set->has_state()) {
      it.printf(COL1_X + TEXT_OFFSET, GRID_START_Y + ROW_HEIGHT + 10, info, white, display::TextAlign::CENTER_LEFT, "%.0f/%.0f°", nozzle_temp->state, nozzle_temp_set->state);
    } else {
      it.printf(COL1_X + TEXT_OFFSET, GRID_START_Y + ROW_HEIGHT + 10, info, white, display::TextAlign::CENTER_LEFT, "%.0f°", nozzle_temp->state);
    }
  } else {
    it.print(COL1_X + TEXT_OFFSET, GRID_START_Y + ROW_HEIGHT + 10, info, white, display::TextAlign::CENTER_LEFT, "--°");
  }

  it.image(COL2_X, GRID_START_Y + ROW_HEIGHT, bed_small, blue);
  if (bed_temp->has_state()) {
    if (bed_temp_set->has_state()) {
      it.printf(COL2_X + TEXT_OFFSET, GRID_START_Y + ROW_HEIGHT + 10, info, white, display::TextAlign::CENTER_LEFT, "%.0f/%.0f°", bed_temp->state, bed_temp_set->state);
    } else {
      it.printf(COL2_X + TEXT_OFFSET, GRID_START_Y + ROW_HEIGHT + 10, info, white, display::TextAlign::CENTER_LEFT, "%.0f°", bed_temp->state);
    }
  } else {
    it.print(COL2_X + TEXT_OFFSET, GRID_START_Y + ROW_HEIGHT + 10, info, white, display::TextAlign::CENTER_LEFT, "--°");
  }
}

inline void cyd_draw_pagination(display::Display &it, int page, bool media_active, bool printer_on) {
  const int W = 240;
  const int H = 320;
  const int CX = W / 2;
  const int M = 14;

  int total_dots = 3;
  if (media_active) total_dots++;
  if (printer_on) total_dots++;

  int current_dot = page;
  if (page == 4) {
    current_dot = total_dots - 1;
  } else if (page == 3) {
    current_dot = 3;
  }

  const int DOTS = total_dots;
  const int DOT_RADIUS = 3;
  const int DOT_SPACING = 18;
  const int DOT_Y = H - M - 8;
  const int DOT_START_X = CX - ((DOTS - 1) * DOT_SPACING) / 2;
  for (int i = 0; i < DOTS; i++) {
    int dx = DOT_START_X + i * DOT_SPACING;
    auto col = (i == current_dot) ? blue : grey;
    it.filled_circle(dx, DOT_Y, DOT_RADIUS, col);
  }
}
