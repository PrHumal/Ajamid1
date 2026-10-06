# elektriajam_01_nano

Arduino Nano A000005 (ATmega328P) programm Pololu VNH5019 #1451 draiveri ja
DFRobot FIT0450 alalisvoolumootori juhtimiseks. Enkooderit ei ole ühendatud:
`KIIRUS` määrab PWM-i töötsükli protsendina, mitte mootori tegeliku RPM-i.
Programm ei tea võlli peatumise täpset hetke.

## Kihid

- `mcu/NanoPins.h` sisaldab ainult Nano D7, D8 ja D9 kontaktimääranguid.
- `signal/IDriveOutput.h` kirjeldab draiverist sõltumatut funktsiooniviidete tabelit.
- `signal/Vnh5019Output.cpp` teisendab selle VNH5019 INA/INB/PWM signaalideks.
- `devices/IMotor.h` kirjeldab C `struct`-liidese ja olekutüüpe.
- `control/MotorController.cpp` teostab mitteblokeeriva 10 ms rambi, ohutu
  suunavahetuse ning käivitamise ja seiskamise loogika.
- `app/SerialConsole.cpp` valideerib käsud ja väljastab oleku. Rakenduskiht kasutab
  ainult `IMotor` funktsiooniviiteid.
- `elektriajam_01_nano.cpp` koondab alamkaustade `.cpp`-failid, sest Arduino CLI
  kompileerib skeemi automaatselt kindlalt skeemi juurkaustas olevaid lähtefaile.

Kiirendus on 25 ja aeglustus 40 PWM-protsendipunkti sekundis. `SEISKA` vähendab
väljundi nullini ja rakendab seejärel VNH5019 elektrilise pidurduse
(INA=LOW, INB=LOW, PWM=HIGH). `off()` on seevastu vaba veeremine
(INA=LOW, INB=LOW, PWM=LOW). Uus `KAIVITA` vabastab pidurduse enne sõidusignaali.

## Ühendused

| Nano | VNH5019 |
| --- | --- |
| D7 | INA |
| D8 | INB |
| D9 (PWM) | PWM |
| 5V | VDD |
| GND | GND |

VNH5019 `VIN` ühendatakse eraldi 6 V mootoritoitega. Mootor ühendatakse draiveri
mootoriväljundisse. Nano ja draiveri loogikataseme GND peavad olema ühised.

## Arduino CLI

Skeem on Arduino IDE-ga avatav, kuid selle projekti kompileerimiseks ja
üleslaadimiseks kasutatakse Arduino CLI-d.

PowerShellis määra esmalt Arduino CLI täielik tee:

```powershell
$cli = "C:\täielik\tee\arduino-cli.exe"
```

Kompileeri skeem Arduino Nano ATmega328P jaoks:

```powershell
& $cli compile --fqbn arduino:avr:nano:cpu=atmega328 .
```

Laadi programm valitud pordile, näiteks COM10:

```powershell
& $cli upload -p COM10 --fqbn arduino:avr:nano:cpu=atmega328 .
```

Serial Monitori jaoks võib kasutada VS Code'i monitori või Arduino IDE-d:
kiirus **115200 baud**, rea lõpp **Newline**.

## Käsud

Iga käsk sisestatakse eraldi reale:

```text
KAIVITA
SEISKA
SUUND EDASI
SUUND TAGASI
KIIRUS 0..100
OLEK
ABI
```

Käivitamisel on mootor seisatud ja kiiruseade 40%. Vigase käsu või kiiruse korral
väljastatakse veateade.

## Soovitatud katsete järjestus

1. Testi esmalt toite väljalülitatult ühendused ja ühine GND.
2. Hoia mootor vabalt pöörlevana, lülita sisse 6 V toide ja kontrolli `OLEK`.
3. Saada `KIIRUS 20`, seejärel `KAIVITA`; jälgi aeglast kiirendust.
4. Saada `SUUND TAGASI`; kontrolli, et väljund läheb enne suunavahetust nulli.
5. Saada `SEISKA`; oota rambi lõppu ja kontrolli pidurduse käitumist.
6. Saada uuesti `KAIVITA` ning veendu, et pidurdus vabaneb.

Arduino CLI-ga kompileeriti skeem edukalt Arduino Nano sihtplaadile käsuga:

```powershell
& $cli compile --fqbn arduino:avr:nano:cpu=atmega328 .
```
