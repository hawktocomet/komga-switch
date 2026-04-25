# Komga Switch Reader

A Nintendo Switch homebrew client for [Komga](https://komga.org/), a free and open source comics/manga media server.

## Requirements

- A Nintendo Switch running custom firmware (e.g. Atmosphere)
- A running Komga server on your local network
- devkitPro (for building from source)

## Dependencies

- [nlohmann/json](https://github.com/nlohmann/json) — JSON for Modern C++ (MIT License)
- [Roboto](https://fonts.google.com/specimen/Roboto) — Font by Google (Apache 2.0 License)

## Installation

1. Copy the `.nro` file to `/switch/komga-switch/` on your SD card
2. Launch via the Homebrew Menu

## Building from Source

**Option 1 — devkitPro installed locally:**

Install the following devkitPro packages:

```
`switch-dev switch-sdl2 switch-sdl2_image switch-sdl2_ttf switch-curl switch-mbedtls switch-libjpeg-turbo switch-libpng`
```

Then run:

```
make
```

**Option 2 — Docker:**

```
docker run -it --rm   -v "$PWD:/project"   -w /project   devkitpro/devkita64:latest   bash 
```

Then inside the container:

```
git clone https://github.com/hawktocomet/komga-switch.git
cd komga-switch/
make
````

## Controls
## Controls

### Landscape Mode

| Button | Action |
|---|---|
| L / R | Previous / Next page |
| Left / Right D-Pad | Previous / Next page |
| ZL / ZR | Zoom out / in |
| Left Stick | Pan |
| Up / Down D-Pad | Pan vertical |
| X | Toggle HUD |
| Minus | Switch to portrait mode |
| B | Back |
| Plus | Exit |

### Portrait Mode (hold console vertically)

| Button | Action |
|---|---|
| Up / Down D-Pad | Previous / Next page |
| Left / Right D-Pad | Zoom out / in |
| Left Stick | Pan |
| LB | Toggle HUD |
| Minus | Switch to landscape mode |
| B | Back |
| Plus | Exit |

### Menus

| Button | Action |
|---|---|
| Up / Down D-Pad | Navigate |
| A | Select / Login |
| B | Back / Logout |
| Y | Open keyboard |
| Plus | Exit |

## Notes

- Your server must be reachable over HTTP on your local network
- If "Save credentials" is enabled, your username and password are stored in plaintext at `sdmc:/switch/komga-switch/config.json`
- Base path is optional — only needed if Komga is behind a reverse proxy e.g. `/komga`

## License

Copyright (C) 2026 hawktocomet@gmail.com  
Licensed under the [GNU General Public License v3.0](LICENSE)
