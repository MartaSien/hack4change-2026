# Bałtyk Infokiosk

Projekt infokiosku edukacyjno-informacyjnego, składającego się z materiałów z recyklingu i ekranów eInk. Prezentuje informacje o gatunkach roślin i zwierząt zamieszkujących Bałtyk.

## Cechy

- **Niska konsumpcja energii** - ekrany eInk zużywają mało prądu
- **Zdalna kontrola** - komunikacja MQTT z wieloma urządzeniami ESP32
- **Bez kabli** - możliwość zasilania bateryjnego ("off the grid")
- **Bez zanieczyszczenia światłem** - ekrany eInk nie emitują światła
- **Automatyczna rotacja** - wyświetlane informacje zmieniają się co 10 minut

## Architektura

- **Client** (`client.py`) - Aplikacja GUI do zarządzania wyświetlaczami
  - Połączenie z brokerem MQTT
  - Obsługa wielu urządzeń ESP32
  - Rotacja informacji o gatunkach
  - Wysyłanie tekstu do wyświetlaczy

- **Urządzenia** - ESP32 z podłączonymi ekranami eInk
  - Pobierają dane przez MQTT
  - Wyświetlają informacje na ekranie

- **Dane** - `data/baltic-creatures.json` - informacje o gatunkach

## Instalacja

### Wymagania
- Python 3.7+
- Broker MQTT (np. Mosquitto)
- MQTT-compatible ESP32

### Konfiguracja

1. Instalacja zależności:
```bash
pip install -r requirements.txt
```

2. Uruchomienie klienta:
```bash
python client.py
```

3. Konfiguracja MQTT:
   - Wpisz adres brokera (domyślnie: `localhost`)
   - Wpisz port (domyślnie: `1883`)
   - Kliknij "Connect"

4. Ustawienie urządzeń:
   - Wpisz ID urządzeń ESP32 (np. `esp32_1,esp32_2,esp32_3`)
   - Dane będą wysyłane na topiki `creature/esp32_1`, `creature/esp32_2`, itp.

5. Rozpoczęcie rotacji:
   - Kliknij "Start Rotation" aby automatycznie zmieniać informacje co 10 minut
   - Kliknij "Send Now" aby wysłać obecne informacje natychmiast
   - Kliknij "Stop Rotation" aby zatrzymać automatyczną zmianę

## Komunikacja MQTT

### Format wiadomości

Każda wiadomość zawiera dane o gatunku w formacie JSON:

```json
{
  "name": "Zmieraczek Plażowy",
  "info": "Objęty częściową ochroną w Polsce",
  "dangers": "Sprzątanie plaż z kidziny, martwej materii organicznej"
}
```

### Topiki

- `creature/{device_id}` - wysyłanie informacji do konkretnego urządzenia

## Źródła informacji

- [Materiały o Morzu Bałtyckim](https://github.com/MartaSien/hack4change-2026/issues/3)