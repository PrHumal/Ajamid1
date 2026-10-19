# elektriajam_02_nano

Teine Arduino Nano ülesanne juhib ühe Nano abil kahte püsivalt ühendatud
ajamit: Pololu VNH5019 #1451-ga DC-mootorit ja Pololu DRV8825 #2133-ga
DFRobot FIT0278 samm-mootorit. Mõlemad kontrollerid on eraldi olekuga ning
neid uuendatakse igas `loop()` läbimises.

## Ohutus ja tootjainfo

Enne ühendamist kontrolli klemmide tähendused tootja juhenditest:

- [Pololu VNH5019 #1451](https://www.pololu.com/product/1451) – INA, INB ja
  PWM on loogikasisendid; VDD on loogikatoide, VIN mootori toide.
- [Pololu DRV8825 #2133](https://www.pololu.com/product/2133) – STEP, DIR,
  ENBL, RESET, SLEEP, VMOT, GND ning A1/A2/B1/B2; VMOT on 8,2–45 V.
- [DFRobot FIT0278](https://www.dfrobot.com/product-785.html) – 3,5 V,
  1,7 A faasi kohta, 1,8° samm.

Mootoritoited on eraldi harud: DRV8825 VMOT võib olla näiteks 12 V, kuid
VNH5019 VIN peab vastama konkreetse DC-mootori nimipingele ja jääma draiveri
lubatud vahemikku. Ühtainsat väljundit ei tohi kujutada korraga kahe erineva
pingena. Nano GND, VNH5019 loogika-GND ja DRV8825 mootori-GND peavad olema
ühised. Ühendamata FAULT-i ei tohi ühendada toite külge.

## Täielik ühendusskeem

```text
                                      +---------------- FIT0278 bipolar stepper
                                      |                  coil A -> A1/A2
                                      |                  coil B -> B1/B2
                                      |                  (pairs found with multimeter)
                                      |
  +-------------------+               |   +----------------------------+
  | Arduino Nano      |               +---| A1/A2, B1/B2                |
  |                   |                   | DRV8825 #2133                |
  | D2 -------------- | STEP              | STEP <- D2                    |
  | D3 -------------- | DIR               | DIR  <- D3                    |
  | D4 -------------- | ENBL              | ENBL <- D4                    |
  | 5V -------------- +-------------------| RESET, SLEEP                  |
  |                   |                   | FAULT -- NC                    |
  | GND -------------+-------------------| GND                            |
  +--+----------------+                   | VMOT <- +12 V stepper supply  |
     |                                    | GND -------------+             |
     |                                    +------------------|--------------+
     |                                                       |              |
     |                                          47 uF or more electrolytic     |
     |                                          directly between VMOT and GND  |
     |
     | D7 -------------------- INA       +----------------------------+
     | D8 -------------------- INB       | VNH5019 #1451              |
     | D9 (PWM) -------------- PWM       | OUT1/OUT2 -> DC motor      |
     | 5V -------------------- VDD       | VIN <- DC motor supply     |
     +------------------------ GND       | GND -----------------------+ 
                                        +----------------------------+

  DC motor supply: +V_DC -> VNH5019 VIN; supply negative -> common GND.
  Stepper supply: +12 V (example) -> DRV8825 VMOT; negative -> common GND.
  D4 -- 10 kOhm pull-down -- GND (ENBL is disabled while Nano resets).
```

RESET ja SLEEP on ühendatud Nano 5 V külge. ENBL on aktiivselt madal; D4
10 kΩ tõmbetakisti hoiab selle Nano lähtestamise ajal kõrgel ehk keelatud.
Mõlemad toiteallikate negatiivsed klemmid ühenduvad ühise GND-ga. FIT0278
kahe mähisepaari leidmiseks mõõda juhtmete vahel järjepidevus või takistus:
üks paar läheb A1/A2 ja teine B1/B2. Juhtmevärve ei eeldata.

| Nano / toide | Draiveri klemm | Tähendus |
|---|---|---|
| D7 | VNH5019 INA | DC suund A |
| D8 | VNH5019 INB | DC suund B |
| D9 (PWM) | VNH5019 PWM | DC juhtväärtus |
| 5 V | VNH5019 VDD | loogikatoide |
| D2 | DRV8825 STEP | üks samm tõusva impulsiga |
| D3 | DRV8825 DIR | sammusuund |
| D4 | DRV8825 ENBL | LOW lubab, HIGH keelab |
| 5 V | DRV8825 RESET, SLEEP | draiver lubatud |
| ühendamata | DRV8825 FAULT | teadlikult NC |
| Nano GND | mõlema draiveri GND | ühine signaaliviide |
| DC mootori +V | VNH5019 VIN | eraldi DC-mootoritoide |
| 12 V näide | DRV8825 VMOT | eraldi samm-mootoritoide |
| FIT0278 mähis A | DRV8825 A1/A2 | multimeetriga tuvastatud paar |
| FIT0278 mähis B | DRV8825 B1/B2 | multimeetriga tuvastatud paar |

Eraldi GPIO-d tähendavad, et VNH5019 PWM/INA/INB ja DRV8825 STEP/DIR/ENBL
signaale saab riistvaraliselt sõltumatult muuta. `MOOTOR` valib ainult
Serial-käskude sihtmärgi; see ei puuduta teise mootori olekut. Seetõttu võib
DC-mootor töötada, valida `MOOTOR SAMM` ja käivitada samal ajal samm-mootori.

## DRV8825 voolupiir

Seadista voolupiir enne mootori toite sisselülitamist ja soovitatavalt enne
päiste paigaldamist. Pololu 2133 valem on:

```text
voolupiir [A] = 2 × VREF [V]
VREF = voolupiir / 2
```

FIT0278 nimivool on 1,7 A faasi kohta, seega nimiseade oleks VREF = 0,85 V.
Alusta ohutult madalamalt (näiteks 0,5 V), kontrolli voolu mõõtmise õiget
meetodit ja tõsta ainult vajadusel. Üle ligikaudu 1,5 A faasi kohta vajab
DRV8825 täiendavat jahutust. Ära mõõda toiteploki voolu asemel seda, mida
vajad voolupiiri seadmiseks: õige koht on ühe mähisega järjestikku ühendatud
ampermeeter või VREF mõõtmine tootja juhendi järgi.

DRV8825 VMOT ja GND vahel peab draiveri lähedal olema vähemalt 47 µF
elektrolüütkondensaator. Ühenda polaarsus õigesti. Hoia mootor vabalt
liikuvana, kasuta voolupiiri, mis ei ületa mootori nimivoolu, ning lülita
toide välja enne mähiseühenduste muutmist.

## Koodikihid

- `app/` – Serial-käsud ja valitud mootori viide; draiveri GPIO-sid siin ei ole.
- `control/` – kummagi mootori eraldi ramp, käivitamine, seiskamine,
  suunavahetus ja sammude ajastamine.
- `devices/` – ühine kontekstiga `IMotor` liides.
- `signal/` – VNH5019 INA/INB/PWM ja DRV8825 STEP/DIR/ENBL signaalid.
- `mcu/` – Nano jalgade määrangud.

DC `SEISKA` aeglustab väljundi nulli ja rakendab seejärel VNH5019 elektrilist
pidurdust. Samm-mootori `SEISKA` lõpetab STEP-impulsid, aeglustab rakendatava
juhtväärtuse nulli ja seab ENBL kõrgele; selle järel kaotab samm-mootor
hoidemomendi. Suunavahetus aeglustab ainult vastava mootori nullini, muudab
suuna ja kiirendab uuesti.

STEP-i genereeritakse `micros()` ajastusega. STEP kõrge ja madal tase kestavad
mõlemad vähemalt 1,9 µs (teostuses vähemalt 2 µs); 100% juhtväärtuse juures
on impulsside alguste vahe vähemalt 3800 µs. `KIIRUS` on juhtväärtus, mitte
mõõdetud pöörlemiskiirus; enkoodrit ega PID-regulaatorit ei ole.

## Serial Monitor

Kiirus on **115200 baud**, rea lõpuks `Newline` või `LF`:

```text
MOOTOR DC
MOOTOR SAMM
KIIRUS 0..100
KAIVITA
SEISKA
SUUND EDASI
SUUND TAGASI
OLEK
ABI
```

`OLEK` näitab mõlema mootori olekut, soovitud ja rakendatavat juhtväärtust,
suunda ning aktiivset käsusihtmärki. Vigane käsk annab eestikeelse veateate.

### Sõltumatu samaaegse töö katsekäik

1. Kontrolli pingevabalt kõik ühendused, mähisepaarid, ühine GND,
   kondensaator ja ENBL 10 kΩ tõmme.
2. Sea DRV8825 voolupiir, ühenda mootorid ning hoia võllid vabalt.
3. Saada `MOOTOR DC`, `KIIRUS 30`, `KAIVITA`; DC-mootor hakkab rampiga tööle.
4. Saada `MOOTOR SAMM`, `KIIRUS 40`, `KAIVITA`; samm-mootor hakkab tööle
   ja `OLEK` näitab mõlema olekut.
5. Saada `MOOTOR DC`, `SUUND TAGASI`, seejärel `SEISKA`; ainult DC-mootor
   aeglustub ja pidurdub, samm-mootor peab jätkama.
6. Saada `MOOTOR SAMM`, `SUUND EDASI`, `SEISKA`; samm-impulsid lõpevad ja
   ENBL keelatakse, DC olek ei muutu.

Seda juhendit ja programmi ei ole füüsilise riistvaraga siin kontrollitud;
ülaltoodud katsekäik on ohutu käivitamise ja kontrollimise plaan, mitte väide,
et katse toimus.

## Kompileerimine

Arduino CLI käsk Arduino Nano ATmega328P jaoks:

```powershell
arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328 .
```

Kui `arduino-cli` ei ole PATH-is, kasuta VS Code Arduino Community laienduse
kaustast leitud `arduino-cli.exe` täisteed samas käsus.
