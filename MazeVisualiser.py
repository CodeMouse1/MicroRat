import serial
import time
import sys

# --- KONFIGURATION ---
# Passe diese Werte an dein Setup an:
SERIAL_PORT = 'COM4'  # Oder '/dev/ttyUSB0' oder 'COMx' je nach System und Anschluss
BAUDRATE = 9600     # Muss zur Baudrate deines Mikrokontrollers passen
MAZE_WIDTH = 7        # Breite des Labyrinths (aus deinem Code)
MAZE_HEIGHT = 4       # Höhe des Labyrinths (aus deinem Code)
# --------------------

# Lookup-Tabelle für Wände (Bit-Positionen wie in deinem C-Code)
# Bit 0: Norden (1)
# Bit 1: Osten (2)
# Bit 2: Süden (4)
# Bit 3: Westen (8)

def parse_maze_data(lines):
    maze_map = [[0 for _ in range(MAZE_WIDTH)] for _ in range(MAZE_HEIGHT)]
    
    for line in lines:
        # Teile die Zeile am Komma
        parts = line.strip().split(',') # <--- WICHTIG: split(',')
        for part in parts:
            if not part: # Leere Strings ignorieren
                continue
            try:
                y_str_start = part.find('[') + 1
                y_str_end = part.find(']')
                y = int(part[y_str_start:y_str_end])

                x_str_start = part.find('[', y_str_end) + 1
                x_str_end = part.find(']', x_str_start)
                x = int(part[x_str_start:x_str_end])

                # Der Wert kommt direkt nach dem Doppelpunkt
                value_start = part.find(':', x_str_end) + 1 # <--- WICHTIG: NUR +1, kein Leerzeichen
                value = int(part[value_start:])
                
                if 0 <= y < MAZE_HEIGHT and 0 <= x < MAZE_WIDTH:
                    maze_map[y][x] = value
                else:
                    print(f"Warnung: Ungültige Koordinaten [{y}][{x}] für Wert {value}. Ignoriere.", file=sys.stderr)

            except (ValueError, IndexError) as e:
                print(f"Warnung: Konnte Teil '{part}' nicht parsen. Fehler: {e}", file=sys.stderr)
                continue
    return maze_map

def draw_maze(maze_map):
    """
    Zeichnet das Labyrinth im ASCII-Format basierend auf der 2D-Array-Karte.
    """
    print("\n--- ERFASSTES LABYRINTH ---")
    print("\n")
    
    # Iteriere von der höchsten Y-Koordinate nach unten,
    # um das Labyrinth so darzustellen, dass [0][0] unten links ist.
    for y in range(MAZE_HEIGHT - 1, -1, -1):
        # Erste Zeile: Obere Wände
        line1 = "o"
        for x in range(MAZE_WIDTH):
            cell_value = maze_map[y][x]
            # Bit 0 (Norden)
            if (cell_value & 0b0001) > 0: # Nordwand vorhanden
                line1 += "---o"
            else: # Keine Nordwand
                line1 += "   o"
        print(line1)

        # Zweite Zeile: Vertikale Wände und Zellinhalt (optional)
        line2 = ""
        for x in range(MAZE_WIDTH):
            cell_value = maze_map[y][x]
            # Bit 3 (Westen)
            if (cell_value & 0b1000) > 0: # Westwand vorhanden
                line2 += "|"
            else: # Keine Westwand
                line2 += " "
            
            line2 += "   " # Platz für Zellinhalt (könnte man z.B. X, Y oder Wert reinsetzen)
            
            # Bit 1 (Osten)
            if x == MAZE_WIDTH - 1: # Letzte Zelle in der Reihe
                if (cell_value & 0b0010) > 0: # Ostwand vorhanden
                    line2 += "|"
                else: # Keine Ostwand
                    line2 += " "
        print(line2)

    # Letzte Zeile: Untere Wände der untersten Reihe (y=0)
    line3 = "o"
    for x in range(MAZE_WIDTH):
        cell_value = maze_map[0][x] # Für die unterste Reihe
        # Bit 2 (Süden)
        if (cell_value & 0b0100) > 0: # Südwand vorhanden
            line3 += "---o"
        else: # Keine Südwand
            line3 += "   o"
    print(line3)


# --- HAUPTPROGRAMM ---
def main():
    try:
        ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=1)
        print(f"Verbunden mit {SERIAL_PORT} @ {BAUDRATE} Baud.")
        print("Warte auf 'Labyrinth Karte:'-Header...")

        maze_data_lines = []
        reading_maze = False

        while True:
            try:
                if ser.in_waiting > 0:
                    line = ser.readline().decode('utf-8', errors='ignore').strip()
                    # print(f"Empfangen: '{line}'") # Debug-Ausgabe: rohe empfangene Zeilen

                    if "Labyrinth Karte:" in line:
                        reading_maze = True
                        maze_data_lines = [] # Alte Daten löschen
                        print("\n[MAPPER] Starte Labyrinth-Einlesen...")
                    elif reading_maze and line:
                        # Überprüfen, ob die Zeile wirklich Daten enthält (z.B. "[")
                        if '[' in line and ']:' in line:
                            maze_data_lines.append(line)
                        else: # Wenn es keine Datenzeile ist, könnte es das Ende sein
                            reading_maze = False
                            if maze_data_lines: # Nur zeichnen, wenn Daten gesammelt wurden
                                print("[MAPPER] Labyrinth-Daten empfangen. Zeichne Karte...")
                                parsed_maze = parse_maze_data(maze_data_lines)
                                draw_maze(parsed_maze)
                            else:
                                print("[MAPPER] Header ohne nachfolgende Daten entdeckt.")
                            maze_data_lines = [] # Reset für nächste Karte

                    elif reading_maze and not line: # Leere Zeile nach Maze-Daten = Ende des Datenblocks
                        reading_maze = False
                        if maze_data_lines: # Nur zeichnen, wenn Daten gesammelt wurden
                            print("[MAPPER] Labyrinth-Daten empfangen. Zeichne Karte...")
                            parsed_maze = parse_maze_data(maze_data_lines)
                            draw_maze(parsed_maze)
                        else:
                            print("[MAPPER] Leere Zeile, aber keine Daten zu zeichnen.")
                        maze_data_lines = [] # Reset für nächste Karte
                
                time.sleep(0.01) # Kurze Pause, um CPU nicht zu überlasten

            except serial.SerialException as se:
                print(f"Serieller Port Fehler: {se}. Versuche erneut in 5 Sekunden...", file=sys.stderr)
                time.sleep(5)
                try:
                    ser = serial.Serial(SERIAL_PORT, BAUDRATE, timeout=1)
                    print("Wieder verbunden.")
                except serial.SerialException as se_reconnect:
                    print(f"Fehler bei der Wiederverbindung: {se_reconnect}", file=sys.stderr)
                    break # Beende, wenn Wiederverbindung fehlschlägt
            except KeyboardInterrupt:
                print("\nProgramm beendet.")
                break
            except Exception as e:
                print(f"Ein unerwarteter Fehler ist aufgetreten: {e}", file=sys.stderr)
                break

    except serial.SerialException as e:
        print(f"Fehler beim Öffnen des seriellen Ports {SERIAL_PORT}: {e}", file=sys.stderr)
        print("Bitte stellen Sie sicher, dass der Port verfügbar ist und der Mikrocontroller angeschlossen ist.", file=sys.stderr)
    except KeyboardInterrupt:
        print("\nProgramm beendet.")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("Serieller Port geschlossen.")

if __name__ == "__main__":
    main()