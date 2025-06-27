    
import serial
import time
import sys

# --- KONFIGURATION ---
# Passe diese Werte an dein Setup an:
SERIAL_PORT = 'COM4'    # Oder '/dev/ttyUSB0' oder 'COMx' je nach System und Anschluss
BAUDRATE = 9600         # Muss zur Baudrate deines Mikrokontrollers passen
MAZE_WIDTH = 7          # Breite des Labyrinths (aus deinem C-Code)
MAZE_HEIGHT = 4         # Höhe des Labyrinths (aus deinem C-Code)
# --------------------

# Lookup-Tabelle für Wände (Bit-Positionen wie in deinem C-Code)
# Bit 0: Norden (1)
# Bit 1: Osten (2)
# Bit 2: Süden (4)
# Bit 3: Westen (8)

def parse_maze_data(lines):
    """
    Parst die empfangenen Zeilen und füllt zwei 2D-Arrays:
    eines für die MazeMap-Werte (Wandinformationen) und
    eines für die DistanceMap-Werte (Flood Fill Distanzen).
    """
    maze_map = [[0 for _ in range(MAZE_WIDTH)] for _ in range(MAZE_HEIGHT)]
    distance_map = [[0 for _ in range(MAZE_WIDTH)] for _ in range(MAZE_HEIGHT)] # NEU: Für Distanzwerte

    for line in lines:
        # Teile die Zeile am Komma (jede Zeile kann mehrere [Y][X]:VAL:DIST, Teile haben)
        parts = line.strip().split(',')
        for part in parts:
            if not part: # Leere Strings ignorieren
                continue
            try:
                # Extrahiere Y-Koordinate
                y_str_start = part.find('[') + 1
                y_str_end = part.find(']')
                y = int(part[y_str_start:y_str_end])

                # Extrahiere X-Koordinate
                x_str_start = part.find('[', y_str_end) + 1
                x_str_end = part.find(']', x_str_start)
                x = int(part[x_str_start:x_str_end])

                # Teile den Rest des Strings bei den Doppelpunkten, um Maze- und Distanzwert zu erhalten
                # Beispiel: ":10:0"
                values_portion = part[part.find(':', x_str_end) + 1:]
                
                # Jetzt die Werte aufsplitten (z.B. "10" und "0")
                values_split = values_portion.split(':')

                if len(values_split) == 2:
                    maze_val = int(values_split[0])
                    dist_val = int(values_split[1])
                else:
                    print(f"Warnung: Unerwartetes Format für Werte in '{part}'. Erwarte 'VAL:DIST'.", file=sys.stderr)
                    continue
                
                if 0 <= y < MAZE_HEIGHT and 0 <= x < MAZE_WIDTH:
                    maze_map[y][x] = maze_val
                    distance_map[y][x] = dist_val # NEU: Distanz speichern
                else:
                    print(f"Warnung: Ungültige Koordinaten [{y}][{x}] für Wert {maze_val}:{dist_val}. Ignoriere.", file=sys.stderr)

            except (ValueError, IndexError) as e:
                print(f"Warnung: Konnte Teil '{part}' nicht parsen. Fehler: {e}", file=sys.stderr)
                continue
    return maze_map, distance_map # NEU: Beide Karten zurückgeben

def draw_maze(maze_map, distance_map):
    """
    Zeichnet das Labyrinth im ASCII-Format basierend auf der 2D-Array-Karte
    und zeigt die Distanzwerte innerhalb der Zellen an.
    """
    print("\n--- ERFASSTES LABYRINTH (mit Distanzen) ---")
    print("\n")
    
    # Iteriere von der höchsten Y-Koordinate nach unten,
    # um das Labyrinth so darzustellen, dass [0][0] unten links ist.
    for y in range(MAZE_HEIGHT - 1, -1, -1):
        # Erste Zeile: Obere Wände
        line1 = "o" # Startecke
        for x in range(MAZE_WIDTH):
            cell_value = maze_map[y][x]
            # Prüfe Nordwand (Bit 0)
            if (cell_value & 0b0001) > 0: # Nordwand vorhanden
                line1 += "---o"
            else: # Keine Nordwand
                line1 += "   o"
        print(line1)

        # Zweite Zeile: Vertikale Wände und Zellinhalt (Distanzwert)
        line2 = ""
        # Zeichne die linke Begrenzung der ersten Zelle in der Reihe (Westwand von [y][0])
        if (maze_map[y][0] & 0b1000) > 0: # Westwand vorhanden (Bit 3)
            line2 += "|"
        else:
            line2 += " "

        for x in range(MAZE_WIDTH):
            cell_maze_value = maze_map[y][x]
            cell_dist_value = distance_map[y][x] 
            
            # Formatiere den Distanzwert für die Zellmitte
            if cell_dist_value == 9999: # UNVISITED_DISTANCE
                line2 += "?? "
            else:
                line2 += f"{cell_dist_value:2d} " # Formatiert auf 2 Stellen (z.B. " 5" oder "12")
            
            # ZUSÄTZLICHE PRÜFUNG: Zeichne die rechte Wand (Ostwand) der aktuellen Zelle (x,y)
            # Diese Wand ist auch die linke Wand der Zelle (x+1, y)
            if (cell_maze_value & 0b0010) > 0: # Ostwand vorhanden (Bit 1)
                line2 += "|"
            else:
                line2 += " "
        print(line2)

    # Letzte Zeile: Untere Wände der untersten Reihe (y=0)
    line3 = "o"
    for x in range(MAZE_WIDTH):
        cell_value = maze_map[0][x] # Nur die Südwände der untersten Reihe sind relevant
        # Prüfe Südwand (Bit 2)
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
                        else: # Wenn es keine Datenzeile ist, könnte es eine leere Zeile am Ende sein
                            reading_maze = False
                            if maze_data_lines: # Nur zeichnen, wenn Daten gesammelt wurden
                                print("[MAPPER] Labyrinth-Daten empfangen. Zeichne Karte...")
                                # NEU: Empfange beide Karten
                                parsed_maze_map, parsed_distance_map = parse_maze_data(maze_data_lines)
                                # NEU: Übergib beide Karten an draw_maze
                                draw_maze(parsed_maze_map, parsed_distance_map)
                            else:
                                print("[MAPPER] Header ohne nachfolgende Daten entdeckt.")
                            maze_data_lines = [] # Reset für nächste Karte

                    elif reading_maze and not line: # Leere Zeile nach Maze-Daten = Ende des Datenblocks
                        reading_maze = False
                        if maze_data_lines: # Nur zeichnen, wenn Daten gesammelt wurden
                            print("[MAPPER] Labyrinth-Daten empfangen. Zeichne Karte...")
                            # NEU: Empfange beide Karten
                            parsed_maze_map, parsed_distance_map = parse_maze_data(maze_data_lines)
                            # NEU: Übergib beide Karten an draw_maze
                            draw_maze(parsed_maze_map, parsed_distance_map)
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