# Ajamid

See kaust sisaldab kümmet praktilist tööd.

## Tööde järjekord

| Töö | Alamkaust | Põhiteema |
|---:|---|---|
| 01 | `elektriajam_01_nano` | Üks alalisvoolumootor, algne kihiline lahendus |
| 02 | `elektriajam_02_*` | `IMotor` seadmeabstraktsioon |
| 03 | `elektriajam_03_*` | Teine mootoritüüp sama liidese taga |
| 04 | `elektriajam_04_*` | PWM-i signaalikihi eraldamine |
| 05 | `elektriajam_05_*` | Kahe mootori ühine PWM-generaator |
| 06 | `elektriajam_06_*` | Ajapõhine `RampController` |
| 07 | `elektriajam_07_*` | Enkooder ja PID-tagasiside |
| 08 | `elektriajam_08_*` | Ühine ohutuspoliitika ja avariiseiskamine |
| 09 | `elektriajam_09_*` | Bare-metal loop ja FreeRTOS-i ülesanded |
| 10 | `elektriajam_10_*` | Portimine teisele mikrokontrollerile |


## Kihiline arhitektuur

Praktiliste tööde ühine siht on hoida järgmised kihid eraldi:

1. **Application** – käsud, diagnostika, kasutusjuhtumid ja ohutuspoliitika.
2. **Device abstraction** – mootori või anduri üldine liides.
3. **Actuator/Sensor control** – ramp, PID ja muu juhtimisloogika.
4. **Signal generation** – PWM, suunad ja muud füüsilised signaalid.
5. **RTOS / scheduling** – ajastamine ja ülesannete prioriteedid, kui töö seda
   vajab.
6. **MCU configuration** – kontaktid, taimerid ja taktsageduse seadistus.


## Praegune valmis töö: `elektriajam_01_nano`

Esimene töö kasutab:

- Arduino Nano A000005, ATmega328P;
- Pololu VNH5019 #1451 mootoridraiverit;
- DFRobot FIT0450 alalisvoolumootorit;
- VNH5019 mootori toiteks eraldi 6 V toidet;
- Nano D7 ühendust INA-ga;
- Nano D8 ühendust INB-ga;
- Nano D9 ühendust PWM-iga;
- Nano 5V ühendust VDD-ga;
- ühist GND-d Nano ja draiveri vahel.

Esimese töö täielik juhend on failis
[`elektriajam_01_nano/README.md`](./elektriajam_01_nano/README.md).

Selles töös on kasutusel järgmised kihid:

- `mcu/NanoPins.h` – Nano kontaktid;
- `signal/` – VNH5019 signaalide teostus;
- `devices/IMotor.h` – mootori üldine C-stiilis liides;
- `control/` – mitteblokeeriv ramp ja suunavahetus;
- `app/` – Serial Monitori käsud.

Enkooder ei ole esimeses töös ühendatud. `KIIRUS` määrab PWM-i töötsükli
protsendina, mitte tegeliku RPM-ina. Võlli täpset peatumise hetke programm ei
tea.

## Arduino CLI töövoog

Projektide kompileerimiseks ja plaadile laadimiseks kasutatakse Arduino CLI-d,
mitte Arduino IDE kompilaatorit. Arduino IDE-d võib kasutada skeemi avamiseks
või Serial Monitoriks.

Kuna `arduino-cli` ei pruugi olla Windowsi `PATH`-is, määra PowerShellis
muutuja `$cli` Arduino CLI täieliku failiteega:

```powershell
$cli = "C:\täielik\tee\arduino-cli.exe"
```

Liigu konkreetse töö alamkausta:

```powershell
Set-Location "C:\Users\priit\kloonid\Ajamid1\elektriajam_01_nano"
```

Kompileeri Nano ATmega328P jaoks:

```powershell
& $cli compile --fqbn arduino:avr:nano:cpu=atmega328 .
```

Laadi programm näiteks pordile COM10:

```powershell
& $cli upload -p COM10 --fqbn arduino:avr:nano:cpu=atmega328 .
```

Plaatide ja portide kontroll:

```powershell
& $cli board list
```

Kui Nano kasutab vana bootloaderit, proovi sihtmärki:

```powershell
& $cli compile --fqbn arduino:avr:nano:cpu=atmega328old .
```

Iga järgmise alamkausta jaoks tuleb kasutada sama töövoogu, kuid valida selle
alamkausta tee ja sobiv sihtplaat.

## Serial Monitor

Esimene töö kasutab:

- porti, millele Nano on ühendatud;
- kiirust `115200 baud`;
- rea lõpuks `Newline` või `LF`.

Toetatud käsud on:

```text
KAIVITA
SEISKA
SUUND EDASI
SUUND TAGASI
KIIRUS 0..100
OLEK
ABI
```

