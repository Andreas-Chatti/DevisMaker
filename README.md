# DevisMaker

A desktop application for generating professional moving quotes, built with **C++17** and **Qt 6**.

DevisMaker streamlines the entire quoting workflow: client intake, AI-assisted inventory, multi-criteria pricing engine, and PDF document generation.

---

## Features

### Client & Address Management
- Complete client form (name, phone, departure and arrival addresses)
- Per-address details: floor, elevator, furniture hoist, parking permit
- **Address autocomplete** powered by OpenStreetMap (Nominatim)
- **Automatic distance calculation** between addresses using real road routing (OSRM)

### AI-Powered Inventory
- Free-text input — describe the items to move in plain language
- **AI analysis** (Groq / LLaMA): automatically extracts objects, volumes, and categories from the description
- Built-in reference database of 300+ furniture items with standard volumes
- Items organized by room (living room, bedroom, kitchen, office...)
- Full manual editing (add, modify, delete objects)

### Pricing Engine
- **Two calculation methods**:
  - **Price per m3** — unit rate applied to total volume
  - **5-line breakdown** — detailed split across labor, truck, distance, packaging, and supplies
- Seasonal pricing (peak season: June 15 – September 15)
- Configurable price presets (high / low season)
- 4 service tiers: Eco, Eco+, Standard, Luxe
- Move types: urban (< 150 km), long distance, groupage
- 5 distance brackets for road moves
- Auto-calculated worker count, truck count, and move duration
- Surcharges: parking, furniture hoist, additional addresses
- Insurance (contractual 0.2% / damage 0.5%), 20% VAT

### PDF Generation
- Professional quote with company branding, client details, and cost breakdown
- Detailed inventory export as PDF
- Customizable HTML templates
- 30% deposit amount calculated automatically

### Company Profile
- Full company info: address, phone, email, SIRET, APE code, VAT number
- Employee / user details
- Persisted in JSON

---

## Tech Stack

| Component | Technology |
|---|---|
| Language | C++17 |
| UI Framework | Qt 6.9 (Widgets, Network, PrintSupport) |
| Build | Visual Studio 2022 (MSVC) |
| AI | Groq API (LLaMA 3.1) |
| Geocoding | OpenStreetMap Nominatim + OSRM |
| PDF Export | QPrinter + HTML templates |
| Storage | JSON, INI (QSettings) |

---

## Architecture

```
DevisMaker/
├── src/
│   ├── ui/             # User interface (MainWindow, dialogs)
│   ├── calculator/     # Quote calculation engine
│   ├── generators/     # PDF document generation
│   ├── ia/             # AI integration (API service + inventory analyzer)
│   ├── inventory/      # Inventory models and moving objects
│   ├── streetmap/      # Geocoding and distance calculation
│   ├── models/         # Data models (Client, Devis)
│   ├── user/           # Company / user profile
│   ├── utils/          # Constants, pricing config, shared types
│   └── main.cpp
```

The project follows a **clear separation of concerns**: each module has a single responsibility (calculation, UI, networking, generation). Components communicate through Qt's **signal-slot** mechanism.

---

## Getting Started

1. Download the latest release (`.exe`) from the [Releases](../../releases) page
2. Run **DevisMaker.exe**
3. On first launch the application automatically creates its working directories under `AppData/Local/` (configuration, templates, prompts, logs)
4. *(Optional)* Add a [Groq API key](https://console.groq.com/) in the settings to enable AI-powered inventory analysis

---

## Interface Overview

The application is organized into **4 tabs**:

1. **Client** — Client info, addresses, and dates with autocomplete
2. **Inventory** — Free-text input + AI analysis, manual editing, PDF export
3. **Parameters** — Pricing configuration, presets, calculation method selection
4. **Results** — Cost summary table and PDF quote generation

---

## Context

Personal project developed while learning C++, held to professional standards: SOLID principles, appropriate design patterns (MVC, Observer, Strategy), memory management via Qt's parent-child ownership, asynchronous networking, and centralized logging.
