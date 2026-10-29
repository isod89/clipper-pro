# Clipper Pro — démarrage rapide (SMK-37)

Clipper Pro 1.3, c'est le synthétiseur Clipper 1.3 (avec les correctifs de SLOOP 2.8.3 et ceux de la 1.3) sur le clavier M-VAVE SMK-37 (Pro, Elite, MKE-P37, Starrykey 37 Play).
Le manuel de Clipper ([DEMARRAGE-RAPIDE-FR.md](DEMARRAGE-RAPIDE-FR.md)) reste valable : chaque commande du FM-1 a sa
place sur le SMK-37. Les commandes en plus (pads, faders, molettes, pédale, touches extrêmes) jouent le synthé.
La référence complète : [CLIPPER-PRO.md](CLIPPER-PRO.md).

## Installer

1. Le clavier doit être en firmware M-VAVE **V016** (sinon : M-UPGRADE d'abord).
2. Ouvrir **https://isod89.github.io/clipper-pro/** dans Chrome ou Edge, ou double-clic sur `INSTALL-CLIPPER-PRO.bat`
   (compile et ouvre l'installateur sur http://localhost:8768).
3. Câble USB de données direct, autres logiciels MIDI fermés : cocher la case, **Installer Clipper Pro**.
4. Bloqué sous Windows pendant le redémarrage ? Fermer Chrome complètement, rouvrir la page, Installer (ça reprend).
   Ou `INSTALLER-USB-PRO.bat` (sans navigateur).

## Les encodeurs

| SMK-37 | Clipper Pro |
|---|---|
| K1 | volume général |
| K2 | SELECT : les pages, le menu |
| K3 | ALGORITHM : le genre de son (BASS, KEYS, ORGN, PAD, LEAD, PLCK, STAB, FX, USER) |
| K4 | PRESETS : les sons (les pads continuent de jouer ; KNOB BANK clignote : les SONS sont là) |
| K5 à K8 | les 4 molettes de l'écran (sur HOME : les réglages principaux du son) |

## Les faders (FADER BANK)

FADER BANK affiche l'écran des banques ; appuyé encore, ou `<` `>`, ou K2 : la banque suivante. PATCH le ferme.
Bouger un fader affiche aussi l'écran, avec la valeur réglée.
Un fader ne prend la main qu'une fois bougé : rien ne saute.

| Banque | Fader 1 | Fader 2 | Fader 3 | Fader 4 |
|---|---|---|---|---|
| **A MACROS** (au départ) | BRITE (brillance) | TIMBR (timbre) | MOTN (mouvement) | SPACE (espace) |
| **B ENV** | ATTACK | DECAY | SUSTAIN | RELEASE |
| **C TONE** | CUTOFF (coupure) | RESONANCE | vitesse du LFO | GLIDE |
| **D MIX** | envoi CHORUS | envoi DELAY | envoi REVERB | LEVEL (volume du son) |

Sur HYBRID, la banque A, ce sont ses macros M1 à M4.

## Les molettes et la pédale

- **PITCH** : le pitch bend (amplitude : GLO > MIDI > BEND ; AUTO = celle du son).
- **MOD** : la molette de modulation (la destination MW de la page MOD : vibrato, filtre…; la source MW de la matrice).
- **PEDAL** : pédale de sustain (n'importe quelle polarité ; la brancher **avant** d'allumer, pied levé).

Elles sortent aussi en MIDI (USB et prise TRS), comme les touches.

## Les pads

- **NOTES** (au départ) : les pads jouent le son. Rangée du bas F3 à F4, rangée du haut G4 à G5, avec la nuance.
  Couleur du genre de son ; plus vifs quand la note sonne, les toniques à moitié. Avec SEL > QNT sur WHITE, les pads
  suivent la gamme (pas de fausse note).
- **SONS** : **KNOB BANK** (allumé ; seul KNOB BANK change les pads : K3 et K4 parcourent les sons pendant que les pads
  jouent, et les SONS ouverts suivent le son jusqu'à sa page). Les pads et l'écran montrent 16 sons d'un genre, chaque
  genre sa couleur, le son actuel en blanc. Un pad charge le son et le fait entendre ; `<` `>` les pages ; KNOB BANK ou
  PATCH referme.
- Un bouton de couche tenu : SEQ = les 16 pas (rangée du haut 1 à 8), SCALE (SEL) = un petit piano (un pad choisit la
  tonalité), ARP = note repeat (un pad tenu se répète sur la grille, enregistré en ratchets), PARA (EDIT) = effacer,
  GLOBE (GLO) = FILL (9), FILL à la mesure (10), TAP tempo (16). FX et BT (SAVE) ne sont pas des couches sur Clipper :
  tenus, les pads et les touches continuent de jouer, et un appui ouvre leurs pages. Les cases qui font quelque chose
  sont allumées faiblement, celle qui est active fort.

## Les boutons

| SMK-37 | Clipper Pro |
|---|---|
| PATCH | HOME (appui : le son ; maintenu : le MENU) |
| PARA | EDIT |
| FX | FX |
| SCALE | SEL (tonalité, gamme, accords) |
| CHORD | ENV (les enveloppes) |
| NOTE REPEAT | LFO |
| ARP | ARP (allumé quand l'arpégiateur tourne) |
| SEQ | SEQ |
| GLOBE | GLO |
| BT | SAVE |
| SEQ PLAY / SEQ REC | PLAY / REC |
| OCTAVE− / OCTAVE+ | OCT− / OCT+ |
| KNOB BANK | les SONS sur les pads |
| FADER BANK | les banques de faders |
| `<` `>` | les pages ; SEQ tenu : les pages de pas |

À l'allumage : **OCTAVE−** tenu = CLIPPER PRO USB RESCUE ; **OCTAVE− + OCTAVE+** = calibration des boutons.
OCTAVE− + OCTAVE+ tenus 5 s à l'arrêt = mode UBOOT (sauvegarde complète de la flash).

## Le menu (PATCH maintenu)

Comme Clipper, avec sur le SMK-37 une section **PADS** (la section LIGHTS du FM-1) : **PAD LIGHTS** = la luminosité des pads (HIGH, MID, LOW), et
**PAD THRESHOLD** = la force qu'il faut pour qu'un pad joue (LOW, MID, HIGH, MAX), et **PAD SCOPE** (OCT+) =
ce que mesure le dernier pad frappé : en blanc la force, en jaune le seuil, en rouge le seuil qu'un nouveau coup doit
dépasser, en vert les notes. Quand un pad joue deux notes en moins de 250 ms, l'image s'arrête sur **DOUBLE** :
prenez une photo de l'écran et envoyez-la (OCT+ recommence, OCT- revient).
Un pad frappé s'allume en blanc un instant. L'éclairage des touches
du FM-1 (KEYS) n'existe pas ici : le SMK-37 n'a pas de touches lumineuses.

## Mémoire

Clipper Pro garde comme Clipper : réglages, son en cours, presets utilisateur, banque FM6. Ils sont partagés avec
SLOOP Pro, comme Clipper et SLOOP sur le FM-1. Tables d'ondes à vous (WAVE USR) : **2** emplacements sur le SMK-37
(USR1, USR2).
